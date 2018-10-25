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
	std::vector<GUIObject*> mainMenuObjects;
	std::vector<GUIObject*> settingsObjects;
	std::vector<GUIObject*> gameInterfaceObjects;

	// Main Menu.
	GUILayout* mainMenuLayout = new GUILayout(&renderer);
	mainMenuObjects.push_back(mainMenuLayout);

	mainMenuLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	mainMenuLayout->setSpace(0);
	mainMenuLayout->setTypeLayout(GUILayout_Type::Horizontal);
	mainMenuLayout->setAlignment(GUILayout_Alignment::Center);

	GUIObject* fillObjects[2];
	for (int i = 0; i < 2; ++i)
	{
		fillObjects[i] = new GUIObject(&renderer);
		fillObjects[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		fillObjects[i]->setLayoutFillPercent(3);
	}

	mainMenuLayout->addChild(fillObjects[0]);

	GUILayout* menuLayout = new GUILayout(&renderer);
	mainMenuLayout->addChild(menuLayout);

	glm::vec2 layoutSize = glm::vec2(270, 342);
	menuLayout->init(NULL, glm::vec2(screenWidth / 2 - layoutSize.x / 2, screenHeight / 2 - layoutSize.y / 2), layoutSize, true);
	menuLayout->setLayoutFillPercent(2);
	menuLayout->setSpace(30);
	menuLayout->setTypeLayout(GUILayout_Type::Vertical);
	menuLayout->setAlignment(GUILayout_Alignment::Center);

	mainMenuLayout->addChild(fillObjects[1]);

	std::string textureNames[] = { "playGameButton", "settingsButton", "quitButton" };
	std::string textureNames_0[] = { "playGameButtonHovered", "settingsButtonHovered", "quitButtonHovered" };
	std::string textureNames_1[] = { "playGameButtonPressed", "settingsButtonPressed", "quitButtonPressed" };

	glm::vec2 spriteSize = glm::vec2(270, 84);
	GUIButton* buttons[3];

	for (int i = 0; i < 3; ++i)
	{
		buttons[i] = new GUIButton(&renderer);
		menuLayout->addChild(buttons[i]);

		buttons[i]->init(res_manager.GetTexture(textureNames[i]), glm::vec2(0.0f, 0.0f), spriteSize, true);
		buttons[i]->setSizeRatio(spriteSize.x / spriteSize.y, true);
		buttons[i]->setMinimumSize(glm::vec2(193, 60));
		buttons[i]->setHoveredTexture(res_manager.GetTexture(textureNames_0[i]));
		buttons[i]->setPressedTexture(res_manager.GetTexture(textureNames_1[i]));
	}

	buttons[0]->setActionCallback(showScene);
	buttons[1]->setActionCallback(showSettings);
	buttons[2]->setActionCallback(quitGame);

	// Settings page.
	GUILayout* settingsLayout = new GUILayout(&renderer);
	settingsObjects.push_back(settingsLayout);

	settingsLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	settingsLayout->setSpace(10);
	settingsLayout->setIndents(glm::vec4(50, 10, 50, 10));
	settingsLayout->setTypeLayout(GUILayout_Type::Vertical);
	settingsLayout->setAlignment(GUILayout_Alignment::Top);

	GUILayout* topLayout = new GUILayout(&renderer);
	settingsLayout->addChild(topLayout);

	topLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	topLayout->setMinimumHeight(50.0f);
	topLayout->setMaximumHeight(50.0f);
	topLayout->setLayoutFillPercent(1);
	topLayout->setSpace(0);
	topLayout->setTypeLayout(GUILayout_Type::Horizontal);
	topLayout->setAlignment(GUILayout_Alignment::Left);

	GUIObject* settingsFrame = new GUIObject(&renderer);
	settingsFrame->init(res_manager.GetTexture("settingsFrame"), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	settingsFrame->setLayoutFillPercent(10);

	settingsLayout->addChild(settingsFrame);

	GUILayout* paramsLayout = new GUILayout(&renderer);
	settingsFrame->addChild(paramsLayout);

	paramsLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	paramsLayout->setSpace(20);
	paramsLayout->setUseParentDimensions(true);
	paramsLayout->setIndents(glm::vec4(20, 20, 20, 20));
	paramsLayout->setTypeLayout(GUILayout_Type::Vertical);
	paramsLayout->setAlignment(GUILayout_Alignment::Top);

	GUILayout* lineObject[10];
	int linePercent[] = { 1, 3 };

	for (int i = 0; i < 5; ++i)
	{
		GUILayout* paramLineLayout = new GUILayout(&renderer);
		paramsLayout->addChild(paramLineLayout);

		paramLineLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		paramLineLayout->setTypeLayout(GUILayout_Type::Horizontal);
		paramLineLayout->setAlignment(GUILayout_Alignment::Left);
		paramLineLayout->setMinimumHeight(50);
		paramLineLayout->setMaximumHeight(50);
		
		for (int j = 0; j < 2; ++j)
		{
			lineObject[i * 2 + j] = new GUILayout(&renderer);
			paramLineLayout->addChild(lineObject[i * 2 + j]);

			lineObject[i * 2 + j]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
			lineObject[i * 2 + j]->setTypeLayout(GUILayout_Type::Horizontal);
			lineObject[i * 2 + j]->setAlignment(GUILayout_Alignment::Left);
			lineObject[i * 2 + j]->setIndents(glm::vec4(5, 0, 5, 0));
			lineObject[i * 2 + j]->setLayoutFillPercent(linePercent[j]);
		}
	}

	std::string textBoxData[] ={
		"MULTISAMPLING",
		"GAMMA CORRECTION",
		"HDR",
		"BLOOM",
		"FULLSCREEN"
	};

	bool params[] = {
		useMultisampling,
		useGammaCorrection,
		useHDR,
		useBloom,
		fullscreenMode
	};

	GUICheckBox* checkBoxes[5];
	for (int i = 0; i < 5; ++i)
	{
		GUITextBox* textBox = new GUITextBox(&renderer);
		lineObject[i * 2]->addChild(textBox);

		textBox->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		textBox->setMinimumHeight(40.0f);
		textBox->setMaximumHeight(40.0f);
		textBox->setText(textBoxData[i]);
		textBox->setTextColor(glm::vec3(0.0, 0.68f, 1.0f));
		textBox->setFont(res_manager.Fonts["SansNarrow"]);
		textBox->setFontShader(&font_shader);
		textBox->setFontBuffers(fontVAO, fontVBO);
		textBox->setTextPos(glm::vec2(10.0f, 30.0f));

		checkBoxes[i] = new GUICheckBox(&renderer);
		lineObject[i * 2 + 1]->addChild(checkBoxes[i]);

		checkBoxes[i]->init(res_manager.GetTexture("checkBox"), glm::vec2(0.0f, 0.0f), glm::vec2(40.0f, 40.0f), false);
		checkBoxes[i]->setSizeRatio(1.0f, true);
		checkBoxes[i]->setHoveredTexture(res_manager.GetTexture("checkBoxHovered"));
		checkBoxes[i]->setPressedTexture(res_manager.GetTexture("checkBoxPressed"));

		GUIObject* checkIcon = new GUIObject(&renderer);
		checkIcon->init(res_manager.GetTexture("checkIcon"), glm::vec2(5.0f, 5.0f), glm::vec2(30.0f, 30.0f), false);
		checkIcon->setUseFixedPosition(true);
		checkBoxes[i]->addChild(checkIcon);

		checkBoxes[i]->setChecked(params[i]);
	}

	checkBoxes[0]->setCheckCallback(enableMultisampling);
	checkBoxes[1]->setCheckCallback(enableGammaCorrection);
	checkBoxes[2]->setCheckCallback(enableHDR);
	checkBoxes[3]->setCheckCallback(enableBloom);
	checkBoxes[4]->setCheckCallback(setFullscreen);

	GUIButton* backMenuButton = new GUIButton(&renderer);
	topLayout->addChild(backMenuButton);

	backMenuButton->init(res_manager.GetTexture("backButton"), glm::vec2(10.0f, 10.0f), glm::vec2(40.0f, 40.0f), false);
	backMenuButton->setSizeRatio(1.0f, true);
	backMenuButton->setHoveredTexture(res_manager.GetTexture("backButtonHovered"));
	backMenuButton->setPressedTexture(res_manager.GetTexture("backButtonPressed"));
	backMenuButton->setActionCallback(backToMainMenu);

	// Game Interface.
	GUILayout* gameLayout = new GUILayout(&renderer);
	gameInterfaceObjects.push_back(gameLayout);

	gameLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	gameLayout->setSpace(10);
	gameLayout->setTypeLayout(GUILayout_Type::Vertical);
	gameLayout->setAlignment(GUILayout_Alignment::Center);

	GUILayout* screenLayouts[2];
	int screenPercents[2] = { 9, 1 };

	for (int i = 0; i < 2; ++i)
	{
		screenLayouts[i] = new GUILayout(&renderer);
		gameLayout->addChild(screenLayouts[i]);

		screenLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		screenLayouts[i]->setSpace(10);
		screenLayouts[i]->setIndents(glm::vec4(10, 10, 10, 10));
		screenLayouts[i]->setTypeLayout(GUILayout_Type::Horizontal);
		screenLayouts[i]->setAlignment(GUILayout_Alignment::Left);
		screenLayouts[i]->setLayoutFillPercent(screenPercents[i]);
		//screenLayouts[i]->setColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.1f)); // debug color.
	}

	GUILayout* screenBottomLayouts[2];
	screenPercents[0] = 1;
	screenPercents[1] = 6;

	for (int i = 0; i < 2; ++i)
	{
		screenBottomLayouts[i] = new GUILayout(&renderer);
		screenLayouts[1]->addChild(screenBottomLayouts[i]);

		screenBottomLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		screenBottomLayouts[i]->setSpace(10);
		screenBottomLayouts[i]->setIndents(glm::vec4(20, 0, 20, 0));
		screenBottomLayouts[i]->setTypeLayout(GUILayout_Type::Horizontal);
		screenBottomLayouts[i]->setAlignment(GUILayout_Alignment::Left);
		screenBottomLayouts[i]->setLayoutFillPercent(screenPercents[i]);
	}

	pHealthBar = new GUIObject(&renderer);
	screenBottomLayouts[0]->addChild(pHealthBar);

	pHealthBar->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 20.0f), true);
	pHealthBar->setMinimumHeight(20.0f);
	pHealthBar->setMaximumHeight(20.0f);
	pHealthBar->setColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	mainMenuLayout->resize();
	settingsLayout->resize();
	gameLayout->resize();

	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::MainMenu, mainMenuObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Settings, settingsObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Game, gameInterfaceObjects));
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
	pSpaceCraft->setHealthChangedCallback(healthBarChanged);

	pLaserRay->init(&base_level, glm::vec2(0, 0), glm::vec2(13, 55), res_manager.GetTexture("laserRayBlue"), glm::vec2(0.0f, -500.0f), false);
	pLaserRay->setObjectType(1);
	
	for (int i = 0; i < 30; ++i)
	{
		GameObject* asteroid = new GameObject();
		asteroid->setHealth(20.0f);
		asteroid->setInitialHealth(20.0f);
		asteroid->setDamage(10.0f);
		asteroid->setExplosionTime(1.0f);
		asteroid->setExplosionSprite(res_manager.GetTexture("explosion"));
		asteroid->setUsePhysics(true);

		asteroid->init(&base_level, glm::vec2(rand() % (screenWidth - 100 + 1) + 50, rand() % (screenHeight / 6 + 400 + 1) - 400),
			glm::vec2(46, 47), res_manager.GetTexture("asteroid"), glm::vec2(rand() % 15, rand() % (30 - 10 + 1) + 10));
		asteroid->InitialRotation = rand() % 360;
		asteroid->Rotation = 10.0f;
	}

	SpacecraftObject* enemySpaceCraft = new SpacecraftObject();
	enemySpaceCraft->init(&base_level, glm::vec2(screenWidth / 2 - 42, 200), glm::vec2(85, 92), res_manager.GetTexture("spacecraftEnemy"), glm::vec2(0.0f, 0.0f));
	enemySpaceCraft->InitialRotation = 180.0f;
	enemySpaceCraft->VelocityScale = glm::vec2(200.0f, 100.0f);
	enemySpaceCraft->setExplosionSprite(res_manager.GetTexture("explosion"));
	enemySpaceCraft->setUseAI(true);
	enemySpaceCraft->setControlVelocityByRotation(true);
	enemySpaceCraft->setTargetEnemy(pSpaceCraft);

	pLaserRay = enemySpaceCraft->getLaserRay();
	pLaserRay->init(&base_level, glm::vec2(0, 0), glm::vec2(13, 55), res_manager.GetTexture("laserRayRed"), glm::vec2(0.0f, -500.0f), false);
	pLaserRay->setObjectType(1);

	base_level.resize();
}