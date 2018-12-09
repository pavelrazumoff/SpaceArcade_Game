/*
	* Space Arcade Game
	* Copyright (C) 2018 Paul Razumov.
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	* Source:
	*  https://github.com/pavelrazumoff/SpaceArcade_Game
*/

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
	std::vector<GUIObject*> creditsObjects;
	std::vector<GUIObject*> gameInterfaceObjects;
	std::vector<GUIObject*> gamePauseInterfaceObjects;
	std::vector<GUIObject*> gameOverInterfaceObjects;
	std::vector<GUIObject*> stationInterfaceObjects;

	// Main Menu.
	GUILayout* mainMenuLayout = new GUILayout(&renderer);
	mainMenuObjects.push_back(mainMenuLayout);

	mainMenuLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	mainMenuLayout->setSpace(0);
	mainMenuLayout->setTypeLayout(GUILayout_Type::Vertical);
	mainMenuLayout->setAlignment(GUILayout_Alignment::Top);

	GUILayout* topMenuLayout = new GUILayout(&renderer);
	mainMenuLayout->addChild(topMenuLayout);

	topMenuLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	topMenuLayout->setSpace(0);
	topMenuLayout->setTypeLayout(GUILayout_Type::Horizontal);
	topMenuLayout->setAlignment(GUILayout_Alignment::Right);
	topMenuLayout->setIndents(glm::vec4(20.0f, 5.0f, 20.0f, 0.0f));
	topMenuLayout->setLayoutFillPercent(1);

	GUIButton* creaditsButton = new GUIButton(&renderer);
	topMenuLayout->addChild(creaditsButton);

	creaditsButton->init(res_manager->GetTexture("creditsButton"), glm::vec2(0.0f, 0.0f), glm::vec2(120.0f, 40.0f), true);
	creaditsButton->setMaximumSize(glm::vec2(120.0f, 40.0f));
	creaditsButton->setMinimumSize(glm::vec2(60.0f, 20.0f));
	creaditsButton->setSizeRatio(3.0f, true);
	creaditsButton->setHoveredTexture(res_manager->GetTexture("creditsButtonHovered"));
	creaditsButton->setPressedTexture(res_manager->GetTexture("creditsButtonPressed"));
	creaditsButton->setActionCallback(showCredits);

	GUILayout* middleMenuLayout = new GUILayout(&renderer);
	mainMenuLayout->addChild(middleMenuLayout);

	middleMenuLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	middleMenuLayout->setSpace(0);
	middleMenuLayout->setTypeLayout(GUILayout_Type::Vertical);
	middleMenuLayout->setAlignment(GUILayout_Alignment::Center);
	middleMenuLayout->setLayoutFillPercent(4);

	GUIObject* gameCaption = new GUIObject(&renderer);
	middleMenuLayout->addChild(gameCaption);

	gameCaption->init(res_manager->GetTexture("gameCaption"), glm::vec2(0.0f, 0.0f), glm::vec2(600.0f, 150.0f), true);
	gameCaption->setMaximumSize(glm::vec2(600.0f, 150.0f));
	gameCaption->setSizeRatio(4.0f, true);

	GUILayout* bottomMenuLayout = new GUILayout(&renderer);
	mainMenuLayout->addChild(bottomMenuLayout);

	bottomMenuLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	bottomMenuLayout->setSpace(0);
	bottomMenuLayout->setTypeLayout(GUILayout_Type::Horizontal);
	bottomMenuLayout->setAlignment(GUILayout_Alignment::Center);
	bottomMenuLayout->setLayoutFillPercent(8);

	GUIObject* fillObjects[2];
	for (int i = 0; i < 2; ++i)
	{
		fillObjects[i] = new GUIObject(&renderer);
		fillObjects[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		fillObjects[i]->setLayoutFillPercent(3);
		fillObjects[i]->setColor(glm::vec4(0.0f));
	}

	bottomMenuLayout->addChild(fillObjects[0]);

	GUILayout* menuLayout = new GUILayout(&renderer);
	bottomMenuLayout->addChild(menuLayout);

	glm::vec2 layoutSize = glm::vec2(270, 342);
	menuLayout->init(NULL, glm::vec2(screenWidth / 2 - layoutSize.x / 2, screenHeight / 2 - layoutSize.y / 2), layoutSize, true);
	menuLayout->setLayoutFillPercent(2);
	menuLayout->setSpace(30);
	menuLayout->setTypeLayout(GUILayout_Type::Vertical);
	menuLayout->setAlignment(GUILayout_Alignment::Top);

	bottomMenuLayout->addChild(fillObjects[1]);

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

	GUILayout* lineObject[8];
	int linePercent[] = { 1, 3 };

	for (int i = 0; i < 4; ++i)
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
		"GAMMA CORRECTION",
		"HDR",
		"BLOOM",
		"FULLSCREEN"
	};

	bool params[] = {
		useGammaCorrection,
		useHDR,
		useBloom,
		fullscreenMode
	};

	GUICheckBox* checkBoxes[4];
	for (int i = 0; i < 4; ++i)
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
		textBox->setTextAlignment(TextAlignment::TextAlignLeft);

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

	checkBoxes[0]->setCheckCallback(enableGammaCorrection);
	checkBoxes[1]->setCheckCallback(enableHDR);
	checkBoxes[2]->setCheckCallback(enableBloom);
	checkBoxes[3]->setCheckCallback(setFullscreen);

	GUIButton* backMenuButton = new GUIButton(&renderer);
	topLayout->addChild(backMenuButton);

	backMenuButton->init(res_manager->GetTexture("backButton"), glm::vec2(10.0f, 10.0f), glm::vec2(40.0f, 40.0f), false);
	backMenuButton->setSizeRatio(1.0f, true);
	backMenuButton->setHoveredTexture(res_manager->GetTexture("backButtonHovered"));
	backMenuButton->setPressedTexture(res_manager->GetTexture("backButtonPressed"));
	backMenuButton->setActionCallback(backToMainMenu);

	// Credits page.
	GUILayout* creditsLayout = new GUILayout(&renderer);
	creditsObjects.push_back(creditsLayout);

	creditsLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	creditsLayout->setSpace(10);
	creditsLayout->setIndents(glm::vec4(50, 10, 50, 10));
	creditsLayout->setTypeLayout(GUILayout_Type::Vertical);
	creditsLayout->setAlignment(GUILayout_Alignment::Center);

	GUILayout* topCreditsLayout = new GUILayout(&renderer);
	creditsLayout->addChild(topCreditsLayout);

	topCreditsLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	topCreditsLayout->setMinimumHeight(50.0f);
	topCreditsLayout->setMaximumHeight(50.0f);
	topCreditsLayout->setLayoutFillPercent(1);
	topCreditsLayout->setSpace(0);
	topCreditsLayout->setTypeLayout(GUILayout_Type::Horizontal);
	topCreditsLayout->setAlignment(GUILayout_Alignment::Left);

	backMenuButton = new GUIButton(&renderer);
	topCreditsLayout->addChild(backMenuButton);

	backMenuButton->init(res_manager->GetTexture("backButton"), glm::vec2(10.0f, 10.0f), glm::vec2(40.0f, 40.0f), false);
	backMenuButton->setSizeRatio(1.0f, true);
	backMenuButton->setHoveredTexture(res_manager->GetTexture("backButtonHovered"));
	backMenuButton->setPressedTexture(res_manager->GetTexture("backButtonPressed"));
	backMenuButton->setActionCallback(backToMainMenu);

	GUILayout* creditsBottomLayout = new GUILayout(&renderer);
	creditsLayout->addChild(creditsBottomLayout);

	creditsBottomLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	creditsBottomLayout->setSpace(20);
	creditsBottomLayout->setIndents(glm::vec4(20, 20, 20, 20));
	creditsBottomLayout->setTypeLayout(GUILayout_Type::Horizontal);
	creditsBottomLayout->setAlignment(GUILayout_Alignment::Center);
	creditsBottomLayout->setLayoutFillPercent(10);

	GUILayout* creditsFillLayouts[3];
	int creditsFillPercents[] = { 1, 2, 1 };

	for (int i = 0; i < 3; ++i)
	{
		creditsFillLayouts[i] = new GUILayout(&renderer);
		creditsBottomLayout->addChild(creditsFillLayouts[i]);

		creditsFillLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		creditsFillLayouts[i]->setSpace(10);
		creditsFillLayouts[i]->setIndents(glm::vec4(20, 20, 20, 20));
		creditsFillLayouts[i]->setTypeLayout(GUILayout_Type::Vertical);
		creditsFillLayouts[i]->setAlignment(GUILayout_Alignment::Center);
		creditsFillLayouts[i]->setLayoutFillPercent(creditsFillPercents[i]);
	}

	std::string credits[] = {
		"CREDITS",
		"Created by Paul Razumov",
		"Programming: Paul Razumov",
		"Game Sprites were taken from the Web",
		"GUI Sprites: Paul Razumov",
		"Main Music Themes by Paul Razumov",
		"Tesla Roadster is a property of Tesla, Inc",
		"Starman suit is a property of Space Exploration Technologies Corp"
	};

	for (int i = 0; i < 8; ++i)
	{
		GUITextBox* creditsBox = new GUITextBox(&renderer);
		creditsFillLayouts[1]->addChild(creditsBox);

		creditsBox->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		creditsBox->setMinimumHeight(40.0f);
		creditsBox->setMaximumHeight(40.0f);
		creditsBox->setText(credits[i]);
		creditsBox->setTextColor(glm::vec3(0.0, 0.68f, 1.0f));
		if(i == 0)
			creditsBox->setFont(res_manager->Fonts["TTLakes30"]);
		else
			creditsBox->setFont(res_manager->Fonts["TTLakes26"]);
		creditsBox->setFontShader(&font_shader);
		creditsBox->setFontBuffers(fontVAO, fontVBO);
		creditsBox->setTextAlignment(TextAlignment::TextAlignCenter);
		creditsBox->setUseClipSpace(false);
	}

	// Game Interface.
	// Whole screen.
	GUILayout* gameLayout = new GUILayout(&renderer);
	gameInterfaceObjects.push_back(gameLayout);

	gameLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	gameLayout->setSpace(10);
	gameLayout->setTypeLayout(GUILayout_Type::Vertical);
	gameLayout->setAlignment(GUILayout_Alignment::Center);

	GUILayout* screenLayouts[3];
	int screenPercents[3] = { 1, 7, 2 };

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

	// Top Layout (Pause, Boss HealthBar and Score Menu).
	GUILayout* screenTopLayout = new GUILayout(&renderer);
	screenLayouts[0]->addChild(screenTopLayout);

	screenTopLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	screenTopLayout->setIndents(glm::vec4(20, 0, 20, 0));
	screenTopLayout->setTypeLayout(GUILayout_Type::Horizontal);
	screenTopLayout->setAlignment(GUILayout_Alignment::Left);
	screenTopLayout->setSpace(10.0f);

	GUILayout* topLayouts[3];
	int topAlignments[] = { GUILayout_Alignment::Left, GUILayout_Alignment::Left, GUILayout_Alignment::Right };
	int topFillPercents[] = { 2, 3, 12 };

	for (int i = 0; i < 3; ++i)
	{
		topLayouts[i] = new GUILayout(&renderer);
		screenTopLayout->addChild(topLayouts[i]);

		topLayouts[i]->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		topLayouts[i]->setIndents(glm::vec4(0, 0, 0, 0));
		topLayouts[i]->setTypeLayout(GUILayout_Type::Horizontal);
		topLayouts[i]->setAlignment(topAlignments[i]);
		topLayouts[i]->setLayoutFillPercent(topFillPercents[i]);
		//topLayouts[i]->setColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
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

	// here we have to create boss enemy healthbar.
	pEnemyBarFrame = new GUIObject(&renderer);
	topLayouts[1]->addChild(pEnemyBarFrame);

	pEnemyBarFrame->init(res_manager->GetTexture("enemyBarFrame"), glm::vec2(0.0f, 0.0f), glm::vec2(360.0f, 40.0f), true);
	pEnemyBarFrame->setMaximumSize(glm::vec2(360.0f, 40.0f));
	pEnemyBarFrame->setSizeRatio(9.0f, true);
	pEnemyBarFrame->setVisible(false);

	GUILayout* enemyBarLayout = new GUILayout(&renderer);
	pEnemyBarFrame->addChild(enemyBarLayout);

	enemyBarLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	enemyBarLayout->setIndents(glm::vec4(0, 0, 0, 0));
	enemyBarLayout->setTypeLayout(GUILayout_Type::Horizontal);
	enemyBarLayout->setAlignment(GUILayout_Alignment::Left);
	enemyBarLayout->setUseParentDimensions(true);

	pEnemyHealthBar = new GUIObject(&renderer);
	enemyBarLayout->addChild(pEnemyHealthBar);

	pEnemyHealthBar->init(res_manager->GetTexture("enemyHealthBar"), glm::vec2(0.0f, 0.0f), glm::vec2(360.0f, 40.0f), true);
	pEnemyHealthBar->setMinimumHeight(5.0f);

	pLevelBox = new GUITextBox(&renderer);
	topLayouts[2]->addChild(pLevelBox);

	pLevelBox->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	pLevelBox->setMaximumWidth(140.0f);
	pLevelBox->setMinimumHeight(40.0f);
	pLevelBox->setMaximumHeight(40.0f);
	pLevelBox->setText("Level: 0");
	pLevelBox->setTextColor(glm::vec3(1.0, 0.47f, 0.0f));
	pLevelBox->setFont(res_manager->Fonts["TTLakes30"]);
	pLevelBox->setFontShader(&font_shader);
	pLevelBox->setFontBuffers(fontVAO, fontVBO);

	pScoreBox = new GUITextBox(&renderer);
	topLayouts[2]->addChild(pScoreBox);

	pScoreBox->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	pScoreBox->setMaximumWidth(140.0f);
	pScoreBox->setMinimumHeight(40.0f);
	pScoreBox->setMaximumHeight(40.0f);
	pScoreBox->setText("Score: 0");
	pScoreBox->setTextColor(glm::vec3(1.0, 0.68f, 0.0f));
	pScoreBox->setFont(res_manager->Fonts["TTLakes30"]);
	pScoreBox->setFontShader(&font_shader);
	pScoreBox->setFontBuffers(fontVAO, fontVBO);
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
		screenBottomLayouts[i]->setSpace(5);
		screenBottomLayouts[i]->setTypeLayout(GUILayout_Type::Vertical);
		screenBottomLayouts[i]->setAlignment(GUILayout_Alignment::Bottom);
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
		if (i == 1)
		{
			GUILayout* coinsLayout = new GUILayout(&renderer);
			screenBottomLayouts[i * 2]->addChild(coinsLayout);

			coinsLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
			coinsLayout->setMinimumHeight(35.0f);
			coinsLayout->setMaximumHeight(35.0f);
			coinsLayout->setSpace(10.0f);
			coinsLayout->setTypeLayout(GUILayout_Type::Horizontal);
			coinsLayout->setAlignment(GUILayout_Alignment::Right);

			// coins text.
			pCoinsBox = new GUITextBox(&renderer);
			coinsLayout->addChild(pCoinsBox);

			pCoinsBox->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
			pCoinsBox->setMinimumHeight(35.0f);
			pCoinsBox->setMaximumHeight(35.0f);
			pCoinsBox->setText("0");
			pCoinsBox->setTextColor(glm::vec3(0.23f, 0.63f, 1.0f));
			pCoinsBox->setFont(res_manager->Fonts["TTLakes26"]);
			pCoinsBox->setFontShader(&font_shader);
			pCoinsBox->setFontBuffers(fontVAO, fontVBO);
			pCoinsBox->setTextAlignment(TextAlignment::TextAlignRight);

			GUIObject* coinIcon = new GUIObject(&renderer);
			coinsLayout->addChild(coinIcon);

			coinIcon->init(res_manager->GetTexture("coinIcon"), glm::vec2(0.0f, 0.0f), glm::vec2(32.0f, 32.0f), false);
		}

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

	std::string rocketTexes[] = { "rocketFireCaption", "rocketCreateCaption", "deviceActivateCaption", "ionFireCaption" };

	for (int i = 0; i < 4; ++i)
	{
		GUIObject* rocketCaption = new GUIObject(&renderer);
		rocketsLayout->addChild(rocketCaption);

		rocketCaption->init(res_manager->GetTexture(rocketTexes[i]), glm::vec2(0.0f, 0.0f), glm::vec2(212.0f, 72.0f), true);
		rocketCaption->setMaximumSize(glm::vec2(212.0f, 72.0f));
		rocketCaption->setSizeRatio(2.94f, true);

		if (i == 2)
		{
			rocketCaption->setVisible(false);
			pActivateDeviceCaption = rocketCaption;
		}

		if (i == 3)
		{
			rocketCaption->setVisible(false);
			pIonFireCaption = rocketCaption;
		}
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
	pFinalScore->setTextAlignment(TextAlignment::TextAlignCenter);

	GUIButton* playAgainButton = new GUIButton(&renderer);
	gameOverLayout->addChild(playAgainButton);

	playAgainButton->init(res_manager->GetTexture("playAgainButton"), glm::vec2(0.0f, 0.0f), glm::vec2(300.0f, 100.0f), true);
	playAgainButton->setMaximumSize(glm::vec2(300.0f, 100.0f));
	playAgainButton->setSizeRatio(3.0f, true);
	playAgainButton->setHoveredTexture(res_manager->GetTexture("playAgainButtonHovered"));
	playAgainButton->setPressedTexture(res_manager->GetTexture("playAgainButtonPressed"));
	playAgainButton->setActionCallback(restartScene);

	// Space Station Interface.
	GUILayout* stationLayout = new GUILayout(&renderer);
	stationInterfaceObjects.push_back(stationLayout);

	stationLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(screenWidth, screenHeight), true);
	stationLayout->setSpace(50);
	stationLayout->setTypeLayout(GUILayout_Type::Vertical);
	stationLayout->setAlignment(GUILayout_Alignment::Center);
	stationLayout->setIndents(glm::vec4(50.0f, 50.0f, 50.0f, 50.0f));

	GUIObject* dialogueInterface = new GUIObject(&renderer);
	stationLayout->addChild(dialogueInterface);

	dialogueInterface->init(res_manager->GetTexture("dialogueInterface"), glm::vec2(0.0f, 0.0f), glm::vec2(800.0f, 360.0f), true);
	dialogueInterface->setMaximumSize(glm::vec2(800.0f, 360.0f));
	dialogueInterface->setSizeRatio(2.22f, true);

	GUILayout* dialogueLayout = new GUILayout(&renderer);
	dialogueInterface->addChild(dialogueLayout);

	dialogueLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
	dialogueLayout->setSpace(10);
	dialogueLayout->setTypeLayout(GUILayout_Type::Horizontal);
	dialogueLayout->setAlignment(GUILayout_Alignment::Left);
	dialogueLayout->setIndents(glm::vec4(8.0f, 8.0f, 8.0f, 8.0f));
	dialogueLayout->setUseParentDimensions(true);

	GUIObject* starmanIcon = new GUIObject(&renderer);
	dialogueLayout->addChild(starmanIcon);

	starmanIcon->init(res_manager->GetTexture("starmanIcon"), glm::vec2(8.0f, 8.0f), glm::vec2(297.0f, 343.0f), true);
	starmanIcon->setMaximumSize(glm::vec2(297.0f, 343.0f));
	starmanIcon->setSizeRatio(0.865f, true);

	// dialogues.
	std::string starmanCaptions[] = {
		"Hey! How can i help you?",
		"What do you want to buy?"
	};

	std::string playerCaptions[2][2] = {
		{
			"I want to look at your goods.",
			"Nothing. Bye!"
		},
		{
			"I changed my mind."
		}
	};

	int dialoguePhrases[2][2] = {
		{
			DialoguePhrase::ShowGoods,
			DialoguePhrase::Bye,
		},
		{
			DialoguePhrase::GoBack
		}
	};

	int numOfDialoguePhrases[] = {
		2, 1
	};

	for (int i = 0; i < 2; ++i)
	{
		GUILayout* textLayout = new GUILayout(&renderer);
		dialogueLayout->addChild(textLayout);

		textLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		textLayout->setSpace(15);
		textLayout->setTypeLayout(GUILayout_Type::Vertical);
		textLayout->setAlignment(GUILayout_Alignment::Top);
		textLayout->setIndents(glm::vec4(10.0f, 0.0f, 0.0f, 0.0f));

		if (i > 0)
		{
			textLayout->setVisible(false);
			textLayout->setSpace(10);
		}

		dialogueTextLayouts.push_back(textLayout);

		GUITextBox* starmanPhrase = new GUITextBox(&renderer);
		textLayout->addChild(starmanPhrase);

		starmanPhrase->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
		starmanPhrase->setMinimumHeight(30.0f);
		starmanPhrase->setMaximumHeight(30.0f);
		starmanPhrase->setText(starmanCaptions[i]);
		starmanPhrase->setTextColor(glm::vec3(0.23f, 0.63f, 1.0f));
		starmanPhrase->setFont(res_manager->Fonts["Unispace18"]);
		starmanPhrase->setFontShader(&font_shader);
		starmanPhrase->setFontBuffers(fontVAO, fontVBO);
		//starmanPhrase->setColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));

		for (int j = 0; j < numOfDialoguePhrases[i]; ++j)
		{
			GUIButton* dialogueButton = new GUIButton(&renderer);
			textLayout->addChild(dialogueButton);

			dialogueButton->init(res_manager->GetTexture("dialogueButton"), glm::vec2(0.0f, 0.0f), glm::vec2(461.0f, 39.0f), true);
			dialogueButton->setMaximumSize(glm::vec2(461.0f, 39.0f));
			dialogueButton->setSizeRatio(11.82f, true);
			dialogueButton->setHoveredTexture(res_manager->GetTexture("dialogueButtonHovered"));
			dialogueButton->setPressedTexture(res_manager->GetTexture("dialogueButtonPressed"));
			dialogueButton->setActionCallback(handleDialogue);
			dialogueButton->setActionID(dialoguePhrases[i][j]);

			GUILayout* buttonLayout = new GUILayout(&renderer);
			dialogueButton->addChild(buttonLayout);

			buttonLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
			buttonLayout->setTypeLayout(GUILayout_Type::Horizontal);
			buttonLayout->setAlignment(GUILayout_Alignment::Left);
			buttonLayout->setIndents(glm::vec4(10.0f, 10.0f, 10.0f, 0.0f));
			buttonLayout->setUseParentDimensions(true);

			GUITextBox* playerPhrase = new GUITextBox(&renderer);
			buttonLayout->addChild(playerPhrase);

			playerPhrase->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
			playerPhrase->setMinimumHeight(30.0f);
			playerPhrase->setMaximumHeight(30.0f);
			playerPhrase->setText(playerCaptions[i][j]);
			playerPhrase->setTextColor(glm::vec3(0.05f, 0.05f, 0.05f));
			playerPhrase->setFont(res_manager->Fonts["Unispace18"]);
			playerPhrase->setFontShader(&font_shader);
			playerPhrase->setFontBuffers(fontVAO, fontVBO);
		}

		// create here merch content.
		if (i == 1)
		{
			GUILayout* merchLayout = new GUILayout(&renderer);
			textLayout->addChild(merchLayout);

			merchLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
			merchLayout->setSpace(5);
			merchLayout->setTypeLayout(GUILayout_Type::Horizontal);
			merchLayout->setAlignment(GUILayout_Alignment::Left);
			merchLayout->setIndents(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
			//merchLayout->setColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));

			std::string merchTexes[2][3] = {
				{
					"ionWeaponButton",
					"ionWeaponButtonHovered",
					"ionWeaponButtonPressed"
				},
				{
					"healthFillButton",
					"healthFillButtonHovered",
					"healthFillButtonPressed"
				}
			};

			std::string merchCaptions[] = {
				"Ion Weapon",
				"Fill Health"
			};

			int merchIds[] = { MerchType::IonWeapon, MerchType::HealthFill };

			for (int j = 0; j < 2; ++j)
			{
				GUILayout* merchButtonLayout = new GUILayout(&renderer);
				merchLayout->addChild(merchButtonLayout);

				merchButtonLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
				merchButtonLayout->setSpace(5);
				merchButtonLayout->setTypeLayout(GUILayout_Type::Vertical);
				merchButtonLayout->setAlignment(GUILayout_Alignment::Top);

				// merch icon.
				GUIButton* merchButton = new GUIButton(&renderer);
				merchButtonLayout->addChild(merchButton);

				merchButton->init(res_manager->GetTexture(merchTexes[j][0]), glm::vec2(0.0f, 0.0f), glm::vec2(152.0f, 164.0f), true);
				merchButton->setMaximumSize(glm::vec2(152.0f, 164.0f));
				merchButton->setSizeRatio(0.923f, true);
				merchButton->setHoveredTexture(res_manager->GetTexture(merchTexes[j][1]));
				merchButton->setPressedTexture(res_manager->GetTexture(merchTexes[j][2]));
				merchButton->setActionCallback(handleMerch);
				merchButton->setActionID(merchIds[j]);

				// merch name.
				GUITextBox* merchCaption = new GUITextBox(&renderer);
				merchButtonLayout->addChild(merchCaption);

				merchCaption->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
				merchCaption->setMinimumHeight(32.0f);
				merchCaption->setMaximumHeight(32.0f);
				merchCaption->setText(merchCaptions[j]);
				merchCaption->setTextColor(glm::vec3(1.0f, 0.28f, 0.0f));
				merchCaption->setFont(res_manager->Fonts["Unispace18"]);
				merchCaption->setFontShader(&font_shader);
				merchCaption->setFontBuffers(fontVAO, fontVBO);
				merchCaption->setTextAlignment(TextAlignment::TextAlignCenter);

				// merch price layout.
				GUILayout* merchPriceLayout = new GUILayout(&renderer);
				merchButtonLayout->addChild(merchPriceLayout);

				merchPriceLayout->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
				merchPriceLayout->setSpace(5);
				merchPriceLayout->setTypeLayout(GUILayout_Type::Horizontal);
				merchPriceLayout->setAlignment(GUILayout_Alignment::Center);

				GUIObject* coinIcon = new GUIObject(&renderer);
				merchPriceLayout->addChild(coinIcon);

				coinIcon->init(res_manager->GetTexture("coinIcon"), glm::vec2(0.0f, 0.0f), glm::vec2(32.0f, 32.0f), false);

				GUITextBox* merchPriceText = new GUITextBox(&renderer);
				merchPriceLayout->addChild(merchPriceText);

				// merch price text.
				merchPriceText->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), true);
				merchPriceText->setMinimumSize(glm::vec2(60.0f, 20.0f));
				merchPriceText->setMaximumSize(glm::vec2(60.0f, 20.0f));
				merchPriceText->setText(std::to_string(merchPrices[j]));
				merchPriceText->setTextColor(glm::vec3(0.23f, 0.63f, 1.0f));
				merchPriceText->setFont(res_manager->Fonts["Unispace14"]);
				merchPriceText->setFontShader(&font_shader);
				merchPriceText->setFontBuffers(fontVAO, fontVBO);
			}
		}
	}

	mainMenuLayout->resize();
	settingsLayout->resize();
	creditsLayout->resize();
	gameLayout->resize();
	gamePauseLayout->resize();
	gameOverLayout->resize();
	stationLayout->resize();

	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::MainMenu, mainMenuObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Settings, settingsObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Credits, creditsObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::Game, gameInterfaceObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::PauseGame, gamePauseInterfaceObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::GameOver, gameOverInterfaceObjects));
	gui_objects.insert(std::pair<int, std::vector<GUIObject*>>(PageType::StationDialogue, stationInterfaceObjects));
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
	base_level->addSound("IonChargeEffect", res_manager->getSoundPath("IonChargeEffect"));
	base_level->addSound("ExplosionEffect", res_manager->getSoundPath("ExplosionEffect"));
	base_level->addSound("ExplosionEffect2", res_manager->getSoundPath("ExplosionEffect2"));
	base_level->addSound("ElectricExplosionEffect", res_manager->getSoundPath("ElectricExplosionEffect"));
	base_level->addSound("GeneratorEffect", res_manager->getSoundPath("GeneratorEffect"));
	base_level->setScoreChangedCallback(scoreChanged);

	pCurrentLevel = base_level;

	StartLevelBehaviour* basicBehaviour = new StartLevelBehaviour(base_level, res_manager);
	basicBehaviour->setTeleportPlayerCallback(teleportObject);
	levelBehaviours.push_back(basicBehaviour);

	secret_level = new GameLevel();
	secret_level->init(res_manager->GetCubemap("NebulaSpace"), res_manager->GetShader("Skybox"), &renderer);
	secret_level->setScreenIndents(glm::vec4(10, 10, 10, 10));
	secret_level->setPlayerRestrictionHeight(screenHeight / 2.5f);
	secret_level->setSoundEnginePointer(res_manager->getSoundEngine());
	secret_level->setScoreChangedCallback(scoreChanged);
	secret_level->addSound("BackgroundSound2", res_manager->getSoundPath("BackgroundSound2"));

	SecretLevelBehaviour* secretBehaviour = new SecretLevelBehaviour(secret_level, res_manager);
	secretBehaviour->setTeleportPlayerCallback(teleportObject);
	levelBehaviours.push_back(secretBehaviour);

	basicBehaviour->setSecretBehaviour(secretBehaviour);
	secretBehaviour->setMainBehaviour(basicBehaviour);

	resetInitialSceneData();
}

void MainApp::resetInitialSceneData()
{
	if (levelBehaviours.size() == 0)
		return;

	StartLevelBehaviour* behavior = dynamic_cast<StartLevelBehaviour*>(levelBehaviours[0]);
	if (!behavior)
		return;
	behavior->setFinishLevelCallback(finishScene);
	behavior->setIterateLevelCallback(iterateScene);
	behavior->setShowEnemyBarCallback(showEnemyBar);

	StartLevelData levelData = behavior->getLevelData();
	levelData.starmanLevel = rand() % (8 - 5) + 5;

	levelData.maxNumOfMeteorites = 30;
	levelData.maxNumOfDebris = 50;
	levelData.maxNumOfHealthKits = 2;
	levelData.maxNumOfBarriers = 16;
	levelData.maxNumOfTeamEnemies = 2;

	levelData.meteoritesZone = glm::vec2(-50.0f, -1000.0f);
	levelData.debrisZone = glm::vec2(-100.0f, -1000.0f);
	levelData.barriersZone = glm::vec2(0.0f, 1000.0f);
	levelData.healthKitsZone = glm::vec2(-1500.0f, -6000.0f);

	levelData.healthKitsFreq = 90.0f;
	levelData.playerSpeed = glm::vec2(400.0f, 200.0f);
	levelData.barriersSpeed = 90.0f;
	levelData.basicEnemySpeed = 200.0f;
	levelData.bossEnemySpeed = 100.0f;
	levelData.maxTimeWithoutShield = 12.0f;

	behavior->setLevelData(levelData);

	resetSecretSceneData();
}

void MainApp::resetSecretSceneData()
{
	SecretLevelBehaviour* secretBehavior = dynamic_cast<SecretLevelBehaviour*>(levelBehaviours[1]);
	if (!secretBehavior)
		return;

	secretBehavior->setFinishLevelCallback(finishScene);

	SecretLevelData secretLevelData = secretBehavior->getLevelData();
	secretLevelData.maxNumOfBlackHoles = 19;
	secretLevelData.blackHolesZone = glm::vec2(-100.0f, -3000.0f);
	secretLevelData.blackHolesSpeed = glm::vec2(50, 100);

	secretBehavior->setLevelData(secretLevelData);
}