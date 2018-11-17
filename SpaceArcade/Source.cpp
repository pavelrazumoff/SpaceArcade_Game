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

void rocketIntegrityChanged(int integrity, int maxIntegrity)
{
	app.updateRocketIntegrity(integrity, maxIntegrity);
}

void scoreChanged(int score)
{
	app.updateScore(score);
}

void showScene()
{
	app.currentPage = PageType::Game;
	app.startGame();
}

void showSettings()
{
	app.currentPage = PageType::Settings;
}

void showCredits()
{
	app.currentPage = PageType::Credits;
}

void quitGame()
{
	glfwSetWindowShouldClose(app.getWindow(), true);
}

void backToMainMenu()
{
	app.currentPage = PageType::MainMenu;
}

void pauseScene()
{
	app.pauseGame();
}

void resumeScene()
{
	app.resumeGame();
}

void finishScene()
{
	app.finishGame();
}

void restartScene()
{
	app.restartGame();
}

void iterateScene()
{
	app.iterateLevel();
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