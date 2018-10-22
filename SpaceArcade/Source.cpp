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

void showScene()
{
	app.startPage = false;
}

void quitGame()
{
	glfwSetWindowShouldClose(app.getWindow(), true);
}