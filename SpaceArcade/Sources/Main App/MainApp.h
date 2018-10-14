#pragma once
#include "../Shaders/Shader.h"
#include "../Miscellaneous/Camera.h"
#include "MainData.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class MainApp
{
public:
	MainApp() {}
	MainApp(int screenWidth, int screenHeight);

	void setScreenSize(int screenWidth, int screenHeight);

	int initWindow();
	void init();
	void initScene();
	void loadShaders();
	void loadTextures();
	void loadFonts();
	void initBuffers();
	void clearBuffers();
	void update();
	void render();
	void drawTextData();
	void resize(int width, int height);

	void drawScene(Shader shader, bool finalDraw);
	void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	// input.
	void processInput(GLFWwindow *window);
	void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	void processMouseMove(GLFWwindow* window, double xpos, double ypos);
	void processMouseClick(GLFWwindow* window, int button, int action, int mods);
	void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

	// buffers generation.
	void generateFontBuffers();

	// framebuffers generation.
	void generateFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useMultisampling, bool useHDR);
	void generateBloomFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useHDR);

	// framebuffers resize.
	void resizeFramebuffer(unsigned int FBO, unsigned int texBuffer, unsigned int RBO, bool useMultisampling, bool useHDR, bool forceRegenerate);
	void resizeBloomFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO, bool useHDR);

	GLFWwindow* getWindow();

	Shader screen_shader;
	Shader skybox_shader;
	Shader final_shader;
	Shader shaderBlur;

	//Font.
	Shader font_shader;

	Camera camera;

private:
	GLFWwindow* window = NULL;

	int screenWidth, screenHeight;

	unsigned int framebuffer, intermediateFBO, finalFBO;
	unsigned int texColorMSBuffer, screenTexture;
	unsigned int rbo, intermediateRBO, finalRBO;

	unsigned int finalTextures[2];

	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];

	unsigned int quadVAO, quadVBO;
	unsigned int skyboxVAO, skyboxVBO;

	GLuint fontVAO, fontVBO;

	unsigned int cubemapTexture;

	unsigned int uboMatrices;

	float deltaTime = 0.0f; // Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float lastMouseX = 400, lastMouseY = 300;
	bool firstMouseUse = true;
	bool lbutton_down = false;
	bool key_pressed = false;

	bool useMultisampling = true;
	bool useGammaCorrection = true;
	bool useHDR = true;
	bool useBloom = true;

	// Fonts.
	std::map<GLchar, Character> Characters;
};