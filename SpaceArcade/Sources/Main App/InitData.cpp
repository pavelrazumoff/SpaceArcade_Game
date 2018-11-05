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
	unsigned int blockSkyIndex = glGetUniformBlockIndex(res_manager->Shaders["Skybox"].getShaderProgram(), "Matrices");

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

		buttons[i]->init(res_manager->GetTexture(textureNames[i]), glm::vec2(0.0f, 0.0f), spriteSize, true);
		buttons[i]->setSizeRatio(spriteSize.x / spriteSize.y, true);
		buttons[i]->setMinimumSize(glm::vec2(193, 60));
		buttons[i]->setHoveredTexture(res_manager->GetTexture(textureNames_0[i]));
		buttons[i]->setPressedTexture(res_manager->GetTexture(textureNames_1[i]));
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
	settingsFrame->init(res_manager->GetTexture("settingsFrame"), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
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
		textBox->setFont(res_manager->Fonts["SansNarrow"]);
		textBox->setFontShader(&font_shader);
		textBox->setFontBuffers(fontVAO, fontVBO);
		textBox->setTextPos(glm::vec2(10.0f, 30.0f));

		checkBoxes[i] = new GUICheckBox(&renderer);
		lineObject[i * 2 + 1]->addChild(checkBoxes[i]);

		checkBoxes[i]->init(res_manager->GetTexture("checkBox"), glm::vec2(0.0f, 0.0f), glm::vec2(40.0f, 40.0f), false);
		checkBoxes[i]->setSizeRatio(1.0f, true);
		checkBoxes[i]->setHoveredTexture(res_manager->GetTexture("checkBoxHovered"));
		checkBoxes[i]->setPressedTexture(res_manager->GetTexture("checkBoxPressed"));

		GUIObject* checkIcon = new GUIObject(&renderer);
		checkIcon->init(res_manager->GetTexture("checkIcon"), glm::vec2(5.0f, 5.0f), glm::vec2(30.0f, 30.0f), false);
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

	backMenuButton->init(res_manager->GetTexture("backButton"), glm::vec2(10.0f, 10.0f), glm::vec2(40.0f, 40.0f), false);
	backMenuButton->setSizeRatio(1.0f, true);
	backMenuButton->setHoveredTexture(res_manager->GetTexture("backButtonHovered"));
	backMenuButton->setPressedTexture(res_manager->GetTexture("backButtonPressed"));
	backMenuButton->setActionCallback(backToMainMenu);

	// Game Interface.
	// Whole screen.
	GUILayout* gameLayout = new GUILayout(&renderer);
	gameInterfaceObjects.push_back(gameLayout);

	gameLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	gameLayout->setSpace(10);
	gameLayout->setTypeLayout(GUILayout_Type::Vertical);
	gameLayout->setAlignment(GUILayout_Alignment::Center);

	GUILayout* screenLayouts[2];
	int screenPercents[2] = { 8, 1 };

	// Top Screen and bottom (health and energy bars).
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

	GUILayout* screenBottomLayouts[3];
	int bottomScreenPercents[3] = { 1, 5, 1 };

	// Left layout (Health), Center (Fill), Right (Energy).
	for (int i = 0; i < 3; ++i)
	{
		screenBottomLayouts[i] = new GUILayout(&renderer);
		screenLayouts[1]->addChild(screenBottomLayouts[i]);

		screenBottomLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		screenBottomLayouts[i]->setIndents(glm::vec4(20, 0, 20, 0));
		screenBottomLayouts[i]->setTypeLayout(GUILayout_Type::Vertical);
		screenBottomLayouts[i]->setAlignment(GUILayout_Alignment::Center);
		screenBottomLayouts[i]->setLayoutFillPercent(bottomScreenPercents[i]);
	}

	std::string barCaptions[] = {
		"Health",
		"Energy"
	};

	std::string captionTexes[] = {
		"healthCaption",
		"energyCaption"
	};

	// Frame + layout with caption and changing stats (health and energy).
	GUILayout* barLayouts[2];
	for (int i = 0; i < 2; ++i)
	{
		GUIObject* barCaption = new GUIObject(&renderer);
		screenBottomLayouts[i * 2]->addChild(barCaption);

		barCaption->init(res_manager->GetTexture(captionTexes[i]), glm::vec2(0.0f, 0.0f), glm::vec2(260.0f, 65.0f), true);
		barCaption->setSizeRatio(4.0f, true);

		GUIObject* barFrame = new GUIObject(&renderer);
		screenBottomLayouts[i * 2]->addChild(barFrame);

		barFrame->init(res_manager->GetTexture("barFrame"), glm::vec2(0.0f, 0.0f), glm::vec2(260.0f, 40.0f), true);
		barFrame->setSizeRatio(6.5f, true);

		barLayouts[i] = new GUILayout(&renderer);
		barFrame->addChild(barLayouts[i]);

		barLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		barLayouts[i]->setIndents(glm::vec4(3, 7, 3, 7));
		barLayouts[i]->setTypeLayout(GUILayout_Type::Horizontal);
		barLayouts[i]->setAlignment(GUILayout_Alignment::Left);
		barLayouts[i]->setUseParentDimensions(true);
	}

	pHealthBar = new GUIObject(&renderer);
	barLayouts[0]->addChild(pHealthBar);

	pHealthBar->init(res_manager->GetTexture("healthBar"), glm::vec2(0.0f, 0.0f), glm::vec2(250.0f, 30.0f), true);
	pHealthBar->setSizeRatio(8.5f, true);

	pEnergyBar = new GUIObject(&renderer);
	barLayouts[1]->addChild(pEnergyBar);

	pEnergyBar->init(res_manager->GetTexture("energyBar"), glm::vec2(0.0f, 0.0f), glm::vec2(250.0f, 30.0f), true);
	pEnergyBar->setSizeRatio(8.5f, true);

	mainMenuLayout->resize();
	settingsLayout->resize();
	gameLayout->resize();

	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::MainMenu, mainMenuObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Settings, settingsObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Game, gameInterfaceObjects));
}

void MainApp::initScene()
{
	base_level = new GameLevel();
	base_level->init(res_manager->GetCubemap("BlueSpace"), res_manager->GetShader("Skybox"), &renderer);
	base_level->setScreenIndents(glm::vec4(10, 10, 10, 10));
	base_level->setPlayerRestrictionHeight(screenHeight / 2.5f);
	base_level->setSoundEnginePointer(res_manager->getSoundEngine());
	base_level->addSound("BackgroundSound", res_manager->getSoundPath("BackgroundSound"));
	base_level->addSound("LaserSound", res_manager->getSoundPath("LaserSound"));
	base_level->addSound("LaserEnemySound", res_manager->getSoundPath("LaserEnemySound"));
	base_level->addSound("ExplosionEffect", res_manager->getSoundPath("ExplosionEffect"));
	base_level->addSound("ExplosionEffect2", res_manager->getSoundPath("ExplosionEffect2"));
	base_level->addSound("ElectricExplosionEffect", res_manager->getSoundPath("ElectricExplosionEffect"));
	base_level->addSound("GeneratorEffect", res_manager->getSoundPath("GeneratorEffect"));

	StartLevelBehaviour* basicBehaviour = new StartLevelBehaviour(base_level);
	levelBehaviours.push_back(basicBehaviour);

	SpacecraftObject* pSpaceCraft = new SpacecraftObject();
	GameObject* pLaserRay = pSpaceCraft->getLaserRay();

	pSpaceCraft->setObjectType(ObjectTypes::SpaceCraft);
	pSpaceCraft->init(base_level, glm::vec2(screenWidth / 2 - 31, screenHeight - 200), glm::vec2(62, 57), res_manager->GetTexture("spacecraft"), glm::vec2(0.0f, 0.0f));
	pSpaceCraft->VelocityScale = glm::vec2(400.0f, 200.0f);
	pSpaceCraft->setExplosionSprite(res_manager->GetTexture("explosion"));
	pSpaceCraft->setHealthChangedCallback(healthBarChanged);
	pSpaceCraft->setEnergyChangedCallback(energyBarChanged);
	pSpaceCraft->setNonPlayerObject(false);
	pSpaceCraft->setLaserSoundName("LaserSound");
	pSpaceCraft->setExplosionSoundName("ExplosionEffect");

	pLaserRay->setObjectType(ObjectTypes::LaserRay);
	pLaserRay->init(base_level, glm::vec2(0, 0), glm::vec2(13, 55), res_manager->GetTexture("laserRayBlue"), glm::vec2(0.0f, -400.0f), false);
	
	for (int i = 0; i < 100; ++i)
	{
		GameObject* asteroid = new GameObject();
		asteroid->setHealth(20.0f);
		asteroid->setInitialHealth(20.0f);
		asteroid->setDamage(10.0f);
		asteroid->setExplosionTime(1.0f);
		asteroid->setExplosionSprite(res_manager->GetTexture("explosion"));
		asteroid->setUsePhysics(true);
		asteroid->setObjectType(ObjectTypes::Meteorite);
		asteroid->setExplosionSoundName("ExplosionEffect2");
		//asteroid->hideFromLevel(true);

		asteroid->init(base_level, glm::vec2(rand() % (screenWidth - 100 + 1) + 50, rand() % (0 + 3000 + 1) - 3000),
			glm::vec2(46, 47), res_manager->GetTexture("asteroid"), glm::vec2(rand() % 15, rand() % (150 - 50 + 1) + 50));

		asteroid->InitialRotation = rand() % 360;
		asteroid->Rotation = 10.0f;
	}

	TeamShipAIController* teamController = new TeamShipAIController();
	aiControllers.push_back(teamController);
	basicBehaviour->addComplexAIController(teamController);
	teamController->setTargetEnemy(pSpaceCraft);

	for (int i = 0; i < 3; ++i)
	{
		BasicShipAIController* spacecraftAI = new BasicShipAIController();
		aiControllers.push_back(spacecraftAI);
		spacecraftAI->setSourcePosition(glm::vec2(0.5f, 0.5f));

		SpacecraftObject* enemySpaceCraft = new SpacecraftObject();
		enemySpaceCraft->init(base_level, glm::vec2(0.0f, 0.0f), glm::vec2(85, 92), res_manager->GetTexture("spacecraftEnemy"), glm::vec2(0.0f, 0.0f));
		enemySpaceCraft->InitialRotation = 180.0f;
		enemySpaceCraft->VelocityScale = glm::vec2(200.0f, 100.0f);
		enemySpaceCraft->setExplosionSprite(res_manager->GetTexture("explosion"));
		enemySpaceCraft->setAIController(spacecraftAI);
		enemySpaceCraft->setControlVelocityByRotation(true);
		enemySpaceCraft->setObjectType(ObjectTypes::SpaceCraft);
		enemySpaceCraft->setHealth(200.0f);
		enemySpaceCraft->setInitialHealth(200.0f);
		enemySpaceCraft->setLaserSoundName("LaserEnemySound");
		enemySpaceCraft->setExplosionSoundName("ExplosionEffect");

		pLaserRay = enemySpaceCraft->getLaserRay();
		pLaserRay->setObjectType(ObjectTypes::LaserRay);
		pLaserRay->init(base_level, glm::vec2(0, 0), glm::vec2(13, 55), res_manager->GetTexture("laserRayRed"), glm::vec2(0.0f, -400.0f), false);

		if(i > 0)
			teamController->addController(spacecraftAI);
	}

	for (int i = 0; i < 10; ++i)
	{
		EnergyBarrierObject* barrier = new EnergyBarrierObject();
		barrier->init(base_level, glm::vec2(rand() % (screenWidth - 170 + 1) + 10, rand() % (0 + 1000 + 1) - 1000),
			glm::vec2(131, 35), res_manager->GetTexture("energyBarrier"), glm::vec2(0.0f, 60.0f));
		barrier->setAnimationDuration(0.5f);
		barrier->hideFromLevel(true);
		barrier->setGeneratorSoundName("GeneratorEffect");

		GameObject* generators[2];
		std::string texes[] = { "leftGenerator", "rightGenerator" };
		for (int j = 0; j < 2; ++j)
		{
			generators[j] = new GameObject();
			generators[j]->setHealth(40.0f);
			generators[j]->setInitialHealth(40.0f);
			generators[j]->setDamage(10.0f);
			generators[j]->setExplosionTime(1.0f);
			generators[j]->setExplosionSprite(res_manager->GetTexture("explosion"));
			generators[j]->setUsePhysics(true);
			generators[j]->setObjectType(ObjectTypes::Basic);
			generators[j]->setExplosionSoundName("ExplosionEffect2");

			generators[j]->init(base_level, glm::vec2(0.0f, 0.0f), glm::vec2(33, 32), res_manager->GetTexture(texes[j]), glm::vec2(0.0f, 0.0f));
		}

		barrier->setGenerators(generators[0], generators[1]);

		GameObject* electricShock = new GameObject();
		electricShock->setDamage(0.1f);
		electricShock->setHealth(1000.0f);
		electricShock->setUsePhysics(false);
		electricShock->setObjectType(ObjectTypes::ElectricShock);
		electricShock->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(96, 84), res_manager->GetTexture("electricShock"), glm::vec2(0.0f, 0.0f));
		electricShock->RelativePosition = glm::vec2(0.0f, 0.0f);
		electricShock->setAnimationDuration(0.5f);
		electricShock->setUseAnimation(true);
		electricShock->setSelfDestroyTime(1.5f);

		barrier->setElectricShock(electricShock);

		BlastWaveObject* blastWave = new BlastWaveObject();
		blastWave->init(base_level, glm::vec2(0.0f, 0.0f), glm::vec2(128, 128), res_manager->GetTexture("blastWave"), glm::vec2(0.0f, 0.0f));
		blastWave->setAnimationDuration(0.5f);
		blastWave->setSelfDestroyTime(0.5f);
		blastWave->setExplosionSoundName("ElectricExplosionEffect");

		barrier->setBlastWave(blastWave);
	}
}