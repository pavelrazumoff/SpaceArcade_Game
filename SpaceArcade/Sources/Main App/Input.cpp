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

void MainApp::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if(currentPage == PageType::Game)
		pCurrentLevel->handleInput(window, deltaTime);
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
		pCurrentLevel->processKey(key, action, &key_pressed);
	
	for (int i = 0; i < gui_objects[currentPage].size(); ++i)
		if (gui_objects[currentPage][i]->isVisible())
			if (gui_objects[currentPage][i]->processKey(key, action))
				break;
}

void MainApp::processMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseUse) // this bool variable is initially set to true.
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouseUse = false;
	}

	for (int i = 0; i < gui_objects[currentPage].size(); ++i)
		if (gui_objects[currentPage][i]->isVisible())
			if (gui_objects[currentPage][i]->processMouseMove(window, xpos, ypos))
				break;

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

	for (int i = 0; i < gui_objects[currentPage].size(); ++i)
		if (gui_objects[currentPage][i]->isVisible())
			if (gui_objects[currentPage][i]->processMouseClick(window, button, action, lastMouseX, lastMouseY))
				break;
}

void MainApp::processMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
}