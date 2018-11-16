#include "MainApp.h"

MainApp::MainApp(int screenWidth, int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

int MainApp::initWindow()
{
	// Here we create window, load GLAD and set some input functions.
	//glfwWindowHint(GLFW_SAMPLES, 4);

	// Create basic window.
	window = glfwCreateWindow(screenWidth, screenHeight, "Space Arcade", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load GLAD, that we'll use to wrap OpenGL function for easy use.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	// Uncomment it to disable cursor showing.
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_click_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
}

void MainApp::setScreenSize(int screenWidth, int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

GLFWwindow* MainApp::getWindow()
{
	return window;
}

void MainApp::init()
{
	// Here we init some base objects, such as camera, buffers, fonts, models and textures.
	camera.init(glm::vec3(0.0f, 1.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	// Enable depth test, backface culling and using multisampling.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	//uncomment it if don't load skyboxes.
	//stbi_set_flip_vertically_on_load(true);

	res_manager = new ResourceManager();
	res_manager->init();

	loadShaders();
	loadTextures();
	loadFonts();
	initBuffers();

	renderer.init(res_manager->GetShader("Sprite"), screenWidth, screenHeight);

	initGUI();
	initScene();
}

void MainApp::loadFonts()
{
	res_manager->loadFont("Fonts/TTLakesCondensed-Bold.otf", "TTLakes26", 26);
	res_manager->loadFont("Fonts/TTLakesCondensed-Bold.otf", "TTLakes30", 30);
}

void MainApp::update()
{
	// Update delta time between two frames.
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	switch (currentPage)
	{
	case PageType::Game:
	case PageType::GameOver:
		base_level->update(deltaTime);
		break;
	case PageType::PauseGame:
		base_level->updatePaused(deltaTime);
		break;
	default:
		break;
	}
}

void MainApp::updateHealthBar(float health, float initialHealth)
{
	if (pHealthBar)
	{
		float percent = health / initialHealth;
		pHealthBar->setClipSpace(glm::vec4(0.0f, 0.0f, 1.0f - percent, 0.0f), true);
	}
}

void MainApp::updateEnergyBar(float usedEnergy, float maxEnergy)
{
	if (pEnergyBar)
	{
		float percent = usedEnergy / maxEnergy;
		pEnergyBar->setClipSpace(glm::vec4(0.0f, 0.0f, 1.0f - percent, 0.0f), true);
	}
}

void MainApp::updateRocketIntegrity(int integrity, int maxIntegrity)
{
	float currentIntegrity = integrity;
	for (int i = 0; i < 3; ++i)
	{
		if (pRocketBars[i])
		{
			if (currentIntegrity >= maxIntegrity)
			{
				pRocketBars[i]->setClipSpace(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), true);
				pRocketBars[i]->setTexture(res_manager->GetTexture("rocketFilledContent"));
			}
			else
			{
				float percent = currentIntegrity / (float)maxIntegrity;
				pRocketBars[i]->setClipSpace(glm::vec4(0.0f, 0.0f, 1.0f - percent, 0.0f), true);
				pRocketBars[i]->setTexture(res_manager->GetTexture("rocketContent"));
			}

			currentIntegrity -= maxIntegrity;
		}
	}
}

void MainApp::updateScore(int score)
{
	std::string textScore = std::to_string(score);
	std::string text = "Score: " + textScore;
	if (pScoreBox)
	{
		pScoreBox->setText(text);
		
		if (score >= 100 && textScore.size() > std::to_string(prevScore).size())
		{
			pScoreBox->setMaximumWidth(pScoreBox->getMaximumWidth() + 15);
			renderer.resize(screenWidth, screenHeight);
			for (int i = 0; i < gui_objects[currentPage].size(); ++i)
				gui_objects[currentPage][i]->resize();
		}
	}

	prevScore = score;
}

void MainApp::resize(int width, int height)
{
	if (width == 0 || height == 0)
		return;

	// Calls when window is resizing.
	screenWidth = width;
	screenHeight = height;

	// Rebuild framebuffers to make them match new window dimensions.
	// Resize main scene framebuffer.
	resizeFramebuffer(framebuffer, texColorMSBuffer, rbo, useMultisampling, useHDR, true);
	// Resize intermediate framebuffer, that used to store framebuffer's rendered scene (used to get around multisampling).
	resizeFramebuffer(intermediateFBO, screenTexture, intermediateRBO, false, useHDR, false);
	// Resize bloom framebuffers (pingpong as also).
	resizeBloomFramebuffer(finalFBO, finalTextures, finalRBO, useHDR);
	for (int i = 0; i < 2; ++i)
		resizeFramebuffer(pingpongFBO[i], pingpongColorbuffers[i], -1, false, useHDR, false);

	renderer.resize(screenWidth, screenHeight);

	for (auto it = gui_objects.begin(); it != gui_objects.end(); ++it)
		for (int i = 0; i < it->second.size(); ++i)
			it->second[i]->resize();

	if(base_level->getLevelStatus() != LevelStatus::NotStarted)
		base_level->resize();
}

void MainApp::setFullscreenMode(bool fullscreen)
{
	fullscreenMode = fullscreen;

	if (fullscreen)
	{
		// backup window position and window size.
		int wndPosX, wndPosY;
		int wndSizeX, wndSizeY;

		glfwGetWindowPos(window, &wndPosX, &wndPosY);
		glfwGetWindowSize(window, &wndSizeX, &wndSizeY);
		lastWndPos = glm::vec2(wndPosX, wndPosY);
		lastWndSize = glm::vec2(wndSizeX, wndSizeY);

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);

		screenWidth = mode->width;
		screenHeight = mode->height;
	}
	else
	{
		// restore last window size and position
		glfwSetWindowMonitor(window, nullptr, lastWndPos.x, lastWndPos.y, lastWndSize.x, lastWndSize.y, 0);
		screenWidth = lastWndSize.x;
		screenHeight = lastWndSize.y;
	}
}

void MainApp::startGame()
{
	base_level->startLevel();
}

void MainApp::pauseGame()
{
	if (base_level)
		base_level->pauseLevel();
	if(pPauseButton)
		pPauseButton->setVisible(false);
	if (pResumeButton)
		pResumeButton->setVisible(true);
	currentPage = PageType::PauseGame;
}

void MainApp::resumeGame()
{
	if (base_level)
		base_level->resumeLevel();
	if (pPauseButton)
		pPauseButton->setVisible(true);
	if (pResumeButton)
		pResumeButton->setVisible(false);
	currentPage = PageType::Game;
}

void MainApp::finishGame()
{
	currentPage = PageType::GameOver;
	base_level->finishLevel();

	int score = base_level->getScore();
	std::string textScore = std::to_string(score);
	std::string text = "Score: " + textScore;
	if (pFinalScore)
	{
		pFinalScore->setText(text);

		pFinalScore->setMaximumWidth(pFinalScore->getMaximumWidth() + 15 * textScore.size());
		renderer.resize(screenWidth, screenHeight);
		for (int i = 0; i < gui_objects[currentPage].size(); ++i)
			gui_objects[currentPage][i]->resize();
	}
}

void MainApp::restartGame()
{
	currentPage = PageType::Game;
	if (pScoreBox)
		pScoreBox->setMaximumWidth(140.0f);
	if (pLevelBox)
		pLevelBox->setMaximumWidth(140.0f);

	renderer.resize(screenWidth, screenHeight);
	for (int i = 0; i < gui_objects[currentPage].size(); ++i)
		gui_objects[currentPage][i]->resize();

	base_level->resetLevel();
	resetInitialSceneData();
	base_level->startLevel();

	prevScore = 0;
	prevLevel = 0;
}

void MainApp::iterateLevel()
{
	if (levelBehaviours.size() == 0)
		return;

	int currentLevel = levelBehaviours[0]->getLevelIteration();
	std::string textLevel = std::to_string(currentLevel);
	std::string text = "Level: " + textLevel;
	if (pLevelBox)
	{
		pLevelBox->setText(text);

		if (currentLevel >= 100 && textLevel.size() > std::to_string(prevLevel).size())
		{
			pLevelBox->setMaximumWidth(pLevelBox->getMaximumWidth() + 15);
			renderer.resize(screenWidth, screenHeight);
			for (int i = 0; i < gui_objects[currentPage].size(); ++i)
				gui_objects[currentPage][i]->resize();
		}
	}

	prevLevel = currentLevel;
}

void MainApp::clearBuffers()
{
	for (auto it = gui_objects.begin(); it != gui_objects.end(); ++it)
		for (int i = 0; i < it->second.size(); ++i)
			delete it->second[i];

	gui_objects.clear();

	delete base_level;
	base_level = NULL;

	for (int i = 0; i < levelBehaviours.size(); ++i)
		delete levelBehaviours[i];

	levelBehaviours.clear();

	delete res_manager;
	res_manager = NULL;

	for (int i = 0; i < 2; ++i)
		glDeleteFramebuffers(1, &pingpongFBO[i]);

	glDeleteFramebuffers(1, &finalFBO);
	glDeleteFramebuffers(1, &intermediateFBO);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &skyboxVBO);
}