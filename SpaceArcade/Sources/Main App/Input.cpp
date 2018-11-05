#include "MainApp.h"

void MainApp::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if(currentPage == PageType::Game)
		base_level->handleInput(window, deltaTime);
}

void MainApp::processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS && !key_pressed)
	{
		useMultisampling = !useMultisampling;
		resize(screenWidth, screenHeight);
		key_pressed = true;
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS && !key_pressed)
	{
		useGammaCorrection = !useGammaCorrection;
		key_pressed = true;
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS && !key_pressed)
	{
		useHDR = !useHDR;
		resize(screenWidth, screenHeight);
		key_pressed = true;
	}

	if (key == GLFW_KEY_4 && action == GLFW_PRESS && !key_pressed)
	{
		useBloom = !useBloom;
		key_pressed = true;
	}

	if (action == GLFW_RELEASE)
	{
		key_pressed = false;
	}

	if (currentPage == PageType::Game)
		base_level->processKey(key, action, &key_pressed);
	else
	{
		for (auto it = gui_objects.begin(); it != gui_objects.end(); ++it)
			for (int i = 0; i < it->second.size(); ++i)
				it->second[i]->processKey(key, action);
	}
}

void MainApp::processMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseUse) // this bool variable is initially set to true.
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouseUse = false;
	}

	if (currentPage != PageType::Game)
	{
		for (int i = 0; i < gui_objects[currentPage].size(); ++i)
			gui_objects[currentPage][i]->processMouseMove(window, xpos, ypos);
	}

	lastMouseX = xpos;
	lastMouseY = ypos;
}

void MainApp::processMouseClick(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS)
			lbutton_down = true;
		else if (action == GLFW_RELEASE)
			lbutton_down = false;
	}

	if (currentPage != PageType::Game)
	{
		for (int i = 0; i < gui_objects[currentPage].size(); ++i)
			gui_objects[currentPage][i]->processMouseClick(window, button, action, lastMouseX, lastMouseY);
	}
}

void MainApp::processMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
}