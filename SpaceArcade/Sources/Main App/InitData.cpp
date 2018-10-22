#include "MainApp.h"

void MainApp::initBuffers()
{
	// screen quad VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// generate Framebuffers.
	generateFramebuffer(&framebuffer, &texColorMSBuffer, &rbo, useMultisampling, useHDR);
	generateFramebuffer(&intermediateFBO, &screenTexture, &intermediateRBO, false, useHDR);
	generateBloomFramebuffer(&finalFBO, finalTextures, &finalRBO, useHDR);

	// ping-pong-framebuffer for blurring
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	// configure a uniform buffer object
	// ---------------------------------
	// first. We get the relevant block indices
	unsigned int blockSkyIndex = glGetUniformBlockIndex(res_manager.Shaders["Skybox"].getShaderProgram(), "Matrices");

	// Now actually create the buffer
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	// Font buffers init.
	generateFontBuffers();
}

void MainApp::initGUI()
{
	GUILayout* mainLayout = new GUILayout(&renderer);
	gui_objects.push_back(mainLayout);

	glm::vec2 layoutSize = glm::vec2(270, 342);
	mainLayout->init(NULL, glm::vec2(screenWidth / 2 - layoutSize.x / 2, screenHeight / 2 - layoutSize.y / 2), layoutSize, true);
	mainLayout->setSpace(30);
	mainLayout->setAlignment(GUILayout_Alignment::Vertical);

	GUIButton* playGameButton = new GUIButton(&renderer);
	mainLayout->addChild(playGameButton);

	glm::vec2 spriteSize = glm::vec2(270, 84);
	playGameButton->init(res_manager.GetTexture("playGameButton"), glm::vec2(screenWidth / 2 - spriteSize.x / 2, screenHeight / 3 - spriteSize.y / 2), spriteSize, true);
	playGameButton->setHoveredTexture(res_manager.GetTexture("playGameButtonHovered"));
	playGameButton->setPressedTexture(res_manager.GetTexture("playGameButtonPressed"));
	playGameButton->resize();

	playGameButton->setActionCallback(showScene);

	GUIButton* settingsButton = new GUIButton(&renderer);
	mainLayout->addChild(settingsButton);

	spriteSize = glm::vec2(270, 84);
	settingsButton->init(res_manager.GetTexture("settingsButton"), glm::vec2(screenWidth / 2 - spriteSize.x / 2, screenHeight / 2 - spriteSize.y / 2), spriteSize, true);
	settingsButton->setHoveredTexture(res_manager.GetTexture("settingsButtonHovered"));
	settingsButton->setPressedTexture(res_manager.GetTexture("settingsButtonPressed"));
	settingsButton->resize();

	GUIButton* quitButton = new GUIButton(&renderer);
	mainLayout->addChild(quitButton);

	spriteSize = glm::vec2(270, 84);
	quitButton->init(res_manager.GetTexture("quitButton"), glm::vec2(screenWidth / 2 - spriteSize.x / 2, (2 * screenHeight) / 3 - spriteSize.y / 2), spriteSize, true);
	quitButton->setHoveredTexture(res_manager.GetTexture("quitButtonHovered"));
	quitButton->setPressedTexture(res_manager.GetTexture("quitButtonPressed"));
	quitButton->resize();

	quitButton->setActionCallback(quitGame);

	mainLayout->resize();
}

void MainApp::initScene()
{
	base_level.init(res_manager.GetCubemap("BlueSpace"), res_manager.GetShader("Skybox"), &renderer);
	base_level.setScreenIndents(glm::vec4(10, 10, 10, 10));
	base_level.setPlayerRestrictionHeight(screenHeight / 2.5f);

	SpacecraftObject* pSpaceCraft = new SpacecraftObject();
	GameObject* pLaserRay = pSpaceCraft->getLaserRay();

	pSpaceCraft->init(&base_level, glm::vec2(screenWidth / 2 - 31, screenHeight - 200), glm::vec2(62, 57), res_manager.GetTexture("spacecraft"), glm::vec2(0.0f, 0.0f));
	pSpaceCraft->VelocityScale = glm::vec2(400.0f, 200.0f);
	pSpaceCraft->setExplosionSprite(res_manager.GetTexture("explosion"));
	pSpaceCraft->setUseAI(false);

	pLaserRay->init(&base_level, glm::vec2(0, 0), glm::vec2(13, 55), res_manager.GetTexture("laserRay"), glm::vec2(0.0f, -500.0f), false);
	pLaserRay->setObjectType(1);
	
	for (int i = 0; i < 30; ++i)
	{
		GameObject* asteroid = new GameObject();
		asteroid->setHealth(20.0f);
		asteroid->setDamage(10.0f);
		asteroid->setExplosionTime(1.0f);
		asteroid->setExplosionSprite(res_manager.GetTexture("explosion"));
		asteroid->setUsePhysics(true);

		asteroid->init(&base_level, glm::vec2(rand() % (screenWidth - 100 + 1) + 50, rand() % (screenHeight / 6 + 400 + 1) - 400),
			glm::vec2(46, 47), res_manager.GetTexture("asteroid"), glm::vec2(rand() % 15, rand() % (30 - 10 + 1) + 10));
		asteroid->InitialRotation = rand() % 360;
		asteroid->Rotation = 5.0f;
	}

	SpacecraftObject* enemySpaceCraft = new SpacecraftObject();
	enemySpaceCraft->init(&base_level, glm::vec2(screenWidth / 2 - 31, 200), glm::vec2(62, 57), res_manager.GetTexture("spacecraft"), glm::vec2(0.0f, 0.0f));
	enemySpaceCraft->InitialRotation = 180.0f;
	enemySpaceCraft->VelocityScale = glm::vec2(200.0f, 100.0f);
	enemySpaceCraft->setExplosionSprite(res_manager.GetTexture("explosion"));
	enemySpaceCraft->setUseAI(true);
	enemySpaceCraft->setControlVelocityByRotation(true);
	enemySpaceCraft->setTargetEnemy(pSpaceCraft);

	pLaserRay = enemySpaceCraft->getLaserRay();
	pLaserRay->init(&base_level, glm::vec2(0, 0), glm::vec2(13, 55), res_manager.GetTexture("laserRay"), glm::vec2(0.0f, -500.0f), false);
	pLaserRay->setObjectType(1);

	base_level.resize();
}