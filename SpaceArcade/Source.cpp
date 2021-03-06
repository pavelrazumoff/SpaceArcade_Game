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

#include "Sources/Main App/MainApp.h"

MainApp app;

int main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	srand(time(0));

	// GLFW initialization.
	// Used to create window, context and manage user input as well.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Uncomment this if uses Mac OS X.
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	app.setScreenSize(1300, 700);
	app.initWindow();

	app.init();

	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// Handle basic input.
		processInput(app.getWindow());

		// Updating.
		app.update();

		// Rendering.
		app.render();

		// Swap buffers and check and call events.
		glfwSwapBuffers(app.getWindow());
		glfwPollEvents();
	}

	app.clearBuffers();

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	app.resize(width, height);
}

void processInput(GLFWwindow *window)
{
	app.processInput(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	app.processKey(window, key, scancode, action, mods);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	app.processMouseMove(window, xpos, ypos);
}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
	app.processMouseClick(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	app.processMouseScroll(window, xoffset, yoffset);
}

void healthBarChanged(float health, float initialHealth)
{
	app.updateHealthBar(health, initialHealth);
}

void energyBarChanged(float usedEnergy, float maxEnergy)
{
	app.updateEnergyBar(usedEnergy, maxEnergy);
}

void enemyHealthBarChanged(float health, float initialHealth)
{
	app.updateEnemyHealthBar(health, initialHealth);
}

void rocketIntegrityChanged(int integrity, int maxIntegrity)
{
	app.updateRocketIntegrity(integrity, maxIntegrity);
}

void blackHolePortalChanged(bool value)
{
	app.updateDeviceActivization(value);
}

void ionWeaponChanged(bool value)
{
	app.updateIonWeaponActivization(value);
}

void coinsChanged(int coins)
{
	app.updateCoins(coins);
}

void scoreChanged(int score)
{
	app.updateScore(score);
}

void showScene(int actionId)
{
	app.currentPage = PageType::Game;
	app.startGame();
}

void showSettings(int actionId)
{
	app.currentPage = PageType::Settings;
}

void showCredits(int actionId)
{
	app.currentPage = PageType::Credits;
}

void showStationDialogue()
{
	app.currentPage = PageType::StationDialogue;
}

void quitGame(int actionId)
{
	glfwSetWindowShouldClose(app.getWindow(), true);
}

void backToMainMenu(int actionId)
{
	app.currentPage = PageType::MainMenu;
}

void pauseScene(int actionId)
{
	app.pauseGame();
}

void resumeScene(int actionId)
{
	app.resumeGame();
}

void finishScene()
{
	app.finishGame();
}

void restartScene(int actionId)
{
	app.restartGame();
}

void iterateScene()
{
	app.iterateLevel();
}

void showEnemyBar(bool show)
{
	app.enableEnemyBar(show);
}

void handleDialogue(int actionId)
{
	app.handleDialogueButton(actionId);
}

void handleMerch(int actionId)
{
	app.handleMerchButton(actionId);
}

void teleportObject(GameObject* obj, LevelBehaviour* behaviour)
{
	app.teleportPlayer(obj, behaviour);
}

void enableMultisampling(bool enable)
{
	app.useMultisampling = enable;
	app.resize(app.screenWidth, app.screenHeight);
}

void enableGammaCorrection(bool enable)
{
	app.useGammaCorrection = enable;
}

void enableHDR(bool enable)
{
	app.useHDR = enable;
	app.resize(app.screenWidth, app.screenHeight);
}

void enableBloom(bool enable)
{
	app.useBloom = enable;
}

void setFullscreen(bool fullscreen)
{
	app.setFullscreenMode(fullscreen);
}