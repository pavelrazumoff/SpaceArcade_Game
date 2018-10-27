#pragma once
#include "../Shaders/Shader.h"
#include "../Miscellaneous/Camera.h"
#include "../Miscellaneous/ResourceManager.h"
#include "Game/GameLevel.h"
#include "GUI/GUICheckBox.h"
#include "GUI/GUILayout.h"
#include "GUI/GUITextBox.h"
#include "MainData.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void healthBarChanged(float health, float initialHealth);
void energyBarChanged(float usedEnergy, float maxEnergy);

void showScene();
void showSettings();
void quitGame();
void backToMainMenu();

void enableMultisampling(bool enable);
void enableGammaCorrection(bool enable);
void enableHDR(bool enable);
void enableBloom(bool enable);
void setFullscreen(bool fullscreen);

enum PageType
{
	MainMenu = 0,
	Settings,
	Game
};

class MainApp
{
public:
	MainApp() {}
	MainApp(int screenWidth, int screenHeight);

	void setScreenSize(int screenWidth, int screenHeight);

	int initWindow();
	void init();
	void initGUI();
	void initScene();
	void initBuffers();

	void loadShaders();
	void loadTextures();
	void loadFonts();

	void clearBuffers();

	void update();

	void resize(int width, int height);

	void render();
	void drawScene();
	void drawSceneInterface();
	void drawMenuBackground();
	void drawStartPage();
	void drawSettingsPage();

	void RenderText(std::string fontType, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	void updateHealthBar(float health, float initialHealth);
	void updateEnergyBar(float usedEnergy, float maxEnergy);

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

	void setFullscreenMode(bool fullscreen);

	GLFWwindow* getWindow();

	Shader font_shader;
	Camera camera;

	// GUI.
	std::map<int, std::vector<GUIObject*>> gui_objects;

	// Game Data.
	GameLevel base_level;

	int currentPage = PageType::MainMenu;

	int screenWidth, screenHeight;

	bool useMultisampling = true;
	bool useGammaCorrection = true;
	bool useHDR = true;
	bool useBloom = true;
	bool fullscreenMode = false;

private:
	GLFWwindow* window = NULL;
	glm::vec2 lastWndPos;
	glm::vec2 lastWndSize;

	unsigned int framebuffer, intermediateFBO, finalFBO;
	unsigned int texColorMSBuffer, screenTexture;
	unsigned int rbo, intermediateRBO, finalRBO;

	unsigned int finalTextures[2];

	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];

	unsigned int quadVAO, quadVBO;
	unsigned int skyboxVAO, skyboxVBO;

	GLuint fontVAO, fontVBO;

	unsigned int uboMatrices;

	float deltaTime = 0.0f; // Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float lastMouseX = 400, lastMouseY = 300;
	bool firstMouseUse = true;
	bool lbutton_down = false;
	bool key_pressed = false;

	ResourceManager res_manager;
	SpriteRenderer renderer;

	GUIObject* pHealthBar = NULL;
	GUIObject* pEnergyBar = NULL;
};