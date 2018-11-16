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
	std::vector<GUIObject*> gamePauseInterfaceObjects;
	std::vector<GUIObject*> gameOverInterfaceObjects;

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
		fillObjects[i]->setColor(glm::vec4(0.0f));
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
		buttons[i] = new GUIButton(&renderer, res_manager->getSoundEngine());
		menuLayout->addChild(buttons[i]);

		buttons[i]->init(res_manager->GetTexture(textureNames[i]), glm::vec2(0.0f, 0.0f), spriteSize, true);
		buttons[i]->setSizeRatio(spriteSize.x / spriteSize.y, true);
		buttons[i]->setMinimumSize(glm::vec2(193, 60));
		buttons[i]->setHoveredTexture(res_manager->GetTexture(textureNames_0[i]));
		buttons[i]->setPressedTexture(res_manager->GetTexture(textureNames_1[i]));
		buttons[i]->addSound("ClickEffect", res_manager->getSoundPath("ClickEffect"));
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
		textBox->setFont(res_manager->Fonts["TTLakes26"]);
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

	GUILayout* screenLayouts[3];
	int screenPercents[3] = { 1, 7, 1 };

	// Top Screen and bottom (health and energy bars).
	for (int i = 0; i < 3; ++i)
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

	// Top Layout (Pause and Score Menu).
	GUILayout* screenTopLayout = new GUILayout(&renderer);
	screenLayouts[0]->addChild(screenTopLayout);

	screenTopLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	screenTopLayout->setIndents(glm::vec4(20, 0, 20, 0));
	screenTopLayout->setTypeLayout(GUILayout_Type::Horizontal);
	screenTopLayout->setAlignment(GUILayout_Alignment::Left);

	GUILayout* topLayouts[2];
	int topAlignments[] = { GUILayout_Alignment::Left, GUILayout_Alignment::Right };
	for (int i = 0; i < 2; ++i)
	{
		topLayouts[i] = new GUILayout(&renderer);
		screenTopLayout->addChild(topLayouts[i]);

		topLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		topLayouts[i]->setIndents(glm::vec4(0, 0, 0, 0));
		topLayouts[i]->setTypeLayout(GUILayout_Type::Horizontal);
		topLayouts[i]->setAlignment(topAlignments[i]);
	}

	pPauseButton = new GUIButton(&renderer);
	topLayouts[0]->addChild(pPauseButton);

	pPauseButton->init(res_manager->GetTexture("pauseButton"), glm::vec2(0.0f, 0.0f), glm::vec2(120.0f, 40.0f), true);
	pPauseButton->setMaximumSize(glm::vec2(120.0f, 40.0f));
	pPauseButton->setMinimumSize(glm::vec2(60.0f, 20.0f));
	pPauseButton->setSizeRatio(3.0f, true);
	pPauseButton->setHoveredTexture(res_manager->GetTexture("pauseButtonHovered"));
	pPauseButton->setPressedTexture(res_manager->GetTexture("pauseButtonPressed"));
	pPauseButton->setActionCallback(pauseScene);

	pLevelBox = new GUITextBox(&renderer);
	topLayouts[1]->addChild(pLevelBox);

	pLevelBox->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	pLevelBox->setMaximumWidth(140.0f);
	pLevelBox->setMinimumHeight(40.0f);
	pLevelBox->setMaximumHeight(40.0f);
	pLevelBox->setText("Level: 0");
	pLevelBox->setTextColor(glm::vec3(1.0, 0.47f, 0.0f));
	pLevelBox->setFont(res_manager->Fonts["TTLakes30"]);
	pLevelBox->setFontShader(&font_shader);
	pLevelBox->setFontBuffers(fontVAO, fontVBO);
	pLevelBox->setTextPos(glm::vec2(10.0f, 30.0f));

	pScoreBox = new GUITextBox(&renderer);
	topLayouts[1]->addChild(pScoreBox);

	pScoreBox->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	pScoreBox->setMaximumWidth(140.0f);
	pScoreBox->setMinimumHeight(40.0f);
	pScoreBox->setMaximumHeight(40.0f);
	pScoreBox->setText("Score: 0");
	pScoreBox->setTextColor(glm::vec3(1.0, 0.68f, 0.0f));
	pScoreBox->setFont(res_manager->Fonts["TTLakes30"]);
	pScoreBox->setFontShader(&font_shader);
	pScoreBox->setFontBuffers(fontVAO, fontVBO);
	pScoreBox->setTextPos(glm::vec2(10.0f, 30.0f));
	//pScoreBox->setColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));

	GUILayout* screenBottomLayouts[3];
	int bottomScreenPercents[3] = { 1, 5, 1 };

	// Left layout (Health), Center (Fill), Right (Energy).
	for (int i = 0; i < 3; ++i)
	{
		screenBottomLayouts[i] = new GUILayout(&renderer);
		screenLayouts[2]->addChild(screenBottomLayouts[i]);

		screenBottomLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		screenBottomLayouts[i]->setIndents(glm::vec4(20, 0, 20, 0));
		screenBottomLayouts[i]->setTypeLayout(GUILayout_Type::Vertical);
		screenBottomLayouts[i]->setAlignment(GUILayout_Alignment::Center);
		screenBottomLayouts[i]->setLayoutFillPercent(bottomScreenPercents[i]);
	}

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
		barLayouts[i]->setIndents(glm::vec4(0, 0, 0, 0));
		barLayouts[i]->setTypeLayout(GUILayout_Type::Horizontal);
		barLayouts[i]->setAlignment(GUILayout_Alignment::Left);
		barLayouts[i]->setUseParentDimensions(true);
	}

	pHealthBar = new GUIObject(&renderer);
	barLayouts[0]->addChild(pHealthBar);

	pHealthBar->init(res_manager->GetTexture("healthBar"), glm::vec2(0.0f, 0.0f), glm::vec2(260.0f, 40.0f), true);
	pHealthBar->setMinimumHeight(5.0f);

	pEnergyBar = new GUIObject(&renderer);
	barLayouts[1]->addChild(pEnergyBar);

	pEnergyBar->init(res_manager->GetTexture("energyBar"), glm::vec2(0.0f, 0.0f), glm::vec2(260.0f, 40.0f), true);
	pEnergyBar->setMinimumHeight(5.0f);

	// Create rocket stats.
	// create horizontal layout for rocket frames.
	GUILayout* rocketsLayout = new GUILayout(&renderer);
	screenBottomLayouts[1]->addChild(rocketsLayout);

	rocketsLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	rocketsLayout->setIndents(glm::vec4(0, 0, 0, 0));
	rocketsLayout->setMinimumHeight(45.0f);
	rocketsLayout->setMaximumHeight(60.0f);
	rocketsLayout->setTypeLayout(GUILayout_Type::Horizontal);
	rocketsLayout->setAlignment(GUILayout_Alignment::Left);
	rocketsLayout->setSpace(20.0f);
	//rocketsLayout->setColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f)); // debug color.

	for (int i = 0; i < 3; ++i)
	{
		GUIObject* rocketFrame = new GUIObject(&renderer);
		rocketsLayout->addChild(rocketFrame);

		rocketFrame->init(res_manager->GetTexture("rocketFrame"), glm::vec2(0.0f, 0.0f), glm::vec2(60.0f, 60.0f), true);
		rocketFrame->setMaximumSize(glm::vec2(60.0f, 60.0f));
		rocketFrame->setSizeRatio(1.0f, true);

		GUILayout* rocketBarLayout = new GUILayout(&renderer);
		rocketFrame->addChild(rocketBarLayout);

		rocketBarLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		rocketBarLayout->setTypeLayout(GUILayout_Type::Horizontal);
		rocketBarLayout->setAlignment(GUILayout_Alignment::Left);
		rocketBarLayout->setUseParentDimensions(true);

		pRocketBars[i] = new GUIObject(&renderer);
		rocketBarLayout->addChild(pRocketBars[i]);

		pRocketBars[i]->init(res_manager->GetTexture("rocketContent"), glm::vec2(0.0f, 0.0f), glm::vec2(60.0f, 60.0f), true);
		pRocketBars[i]->setSizeRatio(1.0f, true);
		pRocketBars[i]->setClipSpace(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), true);
	}

	std::string rocketTexes[] = { "rocketFireCaption", "rocketCreateCaption" };

	for (int i = 0; i < 2; ++i)
	{
		GUIObject* rocketCaption = new GUIObject(&renderer);
		rocketsLayout->addChild(rocketCaption);

		rocketCaption->init(res_manager->GetTexture(rocketTexes[i]), glm::vec2(0.0f, 0.0f), glm::vec2(212.0f, 72.0f), true);
		rocketCaption->setMaximumSize(glm::vec2(212.0f, 72.0f));
		rocketCaption->setSizeRatio(2.94f, true);
	}

	// Game Pause Interface.
	// Whole screen.
	GUILayout* gamePauseLayout = new GUILayout(&renderer);
	gamePauseInterfaceObjects.push_back(gamePauseLayout);

	gamePauseLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	gamePauseLayout->setSpace(10);
	gamePauseLayout->setTypeLayout(GUILayout_Type::Vertical);
	gamePauseLayout->setAlignment(GUILayout_Alignment::Center);
	gamePauseLayout->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));

	GUILayout* screenPauseLayouts[2];
	int screenPausePercents[2] = { 1, 8 };

	// Top Screen and bottom (health and energy bars).
	for (int i = 0; i < 2; ++i)
	{
		screenPauseLayouts[i] = new GUILayout(&renderer);
		gamePauseLayout->addChild(screenPauseLayouts[i]);

		screenPauseLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		screenPauseLayouts[i]->setSpace(10);
		screenPauseLayouts[i]->setIndents(glm::vec4(10, 10, 10, 10));
		screenPauseLayouts[i]->setTypeLayout(GUILayout_Type::Horizontal);
		screenPauseLayouts[i]->setAlignment(GUILayout_Alignment::Left);
		screenPauseLayouts[i]->setLayoutFillPercent(screenPausePercents[i]);
	}

	// Top Layout (Resume Menu).
	GUILayout* screenPauseTopLayout = new GUILayout(&renderer);
	screenPauseLayouts[0]->addChild(screenPauseTopLayout);

	screenPauseTopLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	screenPauseTopLayout->setIndents(glm::vec4(20, 0, 20, 0));
	screenPauseTopLayout->setTypeLayout(GUILayout_Type::Horizontal);
	screenPauseTopLayout->setAlignment(GUILayout_Alignment::Left);

	pResumeButton = new GUIButton(&renderer);
	screenPauseTopLayout->addChild(pResumeButton);

	pResumeButton->init(res_manager->GetTexture("resumeButton"), glm::vec2(0.0f, 0.0f), glm::vec2(120.0f, 40.0f), true);
	pResumeButton->setMaximumSize(glm::vec2(120.0f, 40.0f));
	pResumeButton->setMinimumSize(glm::vec2(60.0f, 20.0f));
	pResumeButton->setSizeRatio(3.0f, true);
	pResumeButton->setHoveredTexture(res_manager->GetTexture("resumeButtonHovered"));
	pResumeButton->setPressedTexture(res_manager->GetTexture("resumeButtonPressed"));
	pResumeButton->setActionCallback(resumeScene);

	// Game Over Interface.
	GUILayout* gameOverLayout = new GUILayout(&renderer);
	gameOverInterfaceObjects.push_back(gameOverLayout);

	gameOverLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	gameOverLayout->setSpace(50);
	gameOverLayout->setTypeLayout(GUILayout_Type::Vertical);
	gameOverLayout->setAlignment(GUILayout_Alignment::Center);
	gameOverLayout->setIndents(glm::vec4(50.0f, 50.0f, 50.0f, 50.0f));

	GUIObject* gameOverCaption = new GUIObject(&renderer);
	gameOverLayout->addChild(gameOverCaption);

	gameOverCaption->init(res_manager->GetTexture("gameOverCaption"), glm::vec2(0.0f, 0.0f), glm::vec2(350.0f, 200.0f), true);
	gameOverCaption->setMaximumSize(glm::vec2(350.0f, 200.0f));
	gameOverCaption->setSizeRatio(1.75f, true);

	pFinalScore = new GUITextBox(&renderer);
	gameOverLayout->addChild(pFinalScore);

	pFinalScore->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	pFinalScore->setMaximumWidth(105.0f);
	pFinalScore->setMinimumHeight(40.0f);
	pFinalScore->setMaximumHeight(40.0f);
	pFinalScore->setText("Score: 0");
	pFinalScore->setTextColor(glm::vec3(1.0, 0.68f, 0.0f));
	pFinalScore->setFont(res_manager->Fonts["TTLakes30"]);
	pFinalScore->setFontShader(&font_shader);
	pFinalScore->setFontBuffers(fontVAO, fontVBO);
	pFinalScore->setTextPos(glm::vec2(10.0f, 30.0f));

	GUIButton* playAgainButton = new GUIButton(&renderer);
	gameOverLayout->addChild(playAgainButton);

	playAgainButton->init(res_manager->GetTexture("playAgainButton"), glm::vec2(0.0f, 0.0f), glm::vec2(300.0f, 100.0f), true);
	playAgainButton->setMaximumSize(glm::vec2(300.0f, 100.0f));
	playAgainButton->setSizeRatio(3.0f, true);
	playAgainButton->setHoveredTexture(res_manager->GetTexture("playAgainButtonHovered"));
	playAgainButton->setPressedTexture(res_manager->GetTexture("playAgainButtonPressed"));
	playAgainButton->setActionCallback(restartScene);

	mainMenuLayout->resize();
	settingsLayout->resize();
	gameLayout->resize();
	gamePauseLayout->resize();
	gameOverLayout->resize();

	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::MainMenu, mainMenuObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Settings, settingsObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Game, gameInterfaceObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::PauseGame, gamePauseInterfaceObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::GameOver, gameOverInterfaceObjects));
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
	base_level->addSound("RocketSound", res_manager->getSoundPath("RocketSound"));
	base_level->addSound("LaserEnemySound", res_manager->getSoundPath("LaserEnemySound"));
	base_level->addSound("ExplosionEffect", res_manager->getSoundPath("ExplosionEffect"));
	base_level->addSound("ExplosionEffect2", res_manager->getSoundPath("ExplosionEffect2"));
	base_level->addSound("ElectricExplosionEffect", res_manager->getSoundPath("ElectricExplosionEffect"));
	base_level->addSound("GeneratorEffect", res_manager->getSoundPath("GeneratorEffect"));
	base_level->setScoreChangedCallback(scoreChanged);

	StartLevelBehaviour* basicBehaviour = new StartLevelBehaviour(base_level, res_manager);
	levelBehaviours.push_back(basicBehaviour);
	
	resetInitialSceneData();
}

void MainApp::resetInitialSceneData()
{
	if (levelBehaviours.size() == 0)
		return;

	StartLevelBehaviour* behavior = (StartLevelBehaviour*)levelBehaviours[0];
	behavior->setFinishLevelCallback(finishScene);
	behavior->setIterateLevelCallback(iterateScene);

	LevelData levelData = behavior->getLevelData();
	levelData.maxNumOfMeteorites = 30;
	levelData.maxNumOfHealthKits = 2;
	levelData.maxNumOfBarriers = 16;
	levelData.maxNumOfTeamEnemies = 2;

	levelData.meteoritesZone = glm::vec2(0.0f, 1000.0f);
	levelData.barriersZone = glm::vec2(0.0f, 1000.0f);
	levelData.healthKitsZone = glm::vec2(-1500.0f, -6000.0f);

	levelData.healthKitsFreq = 90.0f;
	levelData.playerSpeed = glm::vec2(400.0f, 200.0f);
	levelData.barriersSpeed = 90.0f;
	levelData.basicEnemySpeed = 200.0f;
	levelData.bossEnemySpeed = 100.0f;
	levelData.maxTimeWithoutShield = 12.0f;

	behavior->setLevelData(levelData);
}