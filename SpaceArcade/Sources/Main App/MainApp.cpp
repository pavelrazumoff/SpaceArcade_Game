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

	res_manager.init();

	loadShaders();
	loadTextures();
	loadFonts();
	initBuffers();

	renderer.init(res_manager.GetShader("Sprite"), screenWidth, screenHeight);

	initGUI();
	initScene();
}

void MainApp::loadFonts()
{
	res_manager.loadFont("Fonts/PT Sans Narrow.ttf", "SansNarrow", 26);
}

void MainApp::update()
{
	// Update delta time between two frames.
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if(currentPage == PageType::Game)
		base_level.update(deltaTime);
}

void MainApp::resize(int width, int height)
{
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

	base_level.resize();
}

void MainApp::clearBuffers()
{
	for (auto it = gui_objects.begin(); it != gui_objects.end(); ++it)
		for (int i = 0; i < it->second.size(); ++i)
			delete it->second[i];

	gui_objects.clear();

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