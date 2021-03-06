#pragma once
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

#include "../Shaders/Shader.h"
#include "../Miscellaneous/Camera.h"
#include "../Miscellaneous/ResourceManager.h"
#include "Game/GameLevel.h"
#include "Game/AI/TeamShipAIController.h"
#include "Game/AI/StartLevelBehaviour.h"
#include "Game/AI/SecretLevelBehaviour.h"
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

void scoreChanged(int score);

void showScene(int actionId);
void showSettings(int actionId);
void showCredits(int actionId);
void quitGame(int actionId);
void backToMainMenu(int actionId);
void pauseScene(int actionId);
void resumeScene(int actionId);
void finishScene();
void restartScene(int actionId);
void iterateScene();

void showEnemyBar(bool show);

void handleDialogue(int actionId);
void handleMerch(int actionId);

void teleportObject(GameObject* obj, LevelBehaviour* behaviour);

void enableMultisampling(bool enable);
void enableGammaCorrection(bool enable);
void enableHDR(bool enable);
void enableBloom(bool enable);
void setFullscreen(bool fullscreen);

enum PageType
{
	MainMenu = 0,
	Settings,
	Credits,
	Game,
	PauseGame,
	GameOver,
	StationDialogue
};

enum DialoguePhrase
{
	ShowGoods = 0,
	Bye,
	GoBack
};

enum MerchType
{
	IonWeapon = 0,
	HealthFill
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
	void resetInitialSceneData();
	void resetSecretSceneData();
	void initBuffers();

	void loadShaders();
	void loadTextures();
	void loadFonts();
	void loadSounds();

	void clearBuffers();

	void update();

	void resize(int width, int height);

	void render();
	void drawScene();
	void drawMenuBackground();
	void drawPage(int type);

	void RenderText(std::string fontType, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	void updateHealthBar(float health, float initialHealth);
	void updateEnergyBar(float usedEnergy, float maxEnergy);
	void updateEnemyHealthBar(float health, float initialHealth);
	void updateRocketIntegrity(int integrity, int maxIntegrity);
	void updateScore(int score);
	void updateDeviceActivization(bool activate);
	void updateIonWeaponActivization(bool activate);
	void updateCoins(int coins);
	void enableEnemyBar(bool enable);

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

	void startGame();
	void pauseGame();
	void resumeGame();
	void finishGame();
	void restartGame();
	void iterateLevel();

	void handleDialogueButton(int buttonId);
	void handleMerchButton(int merchId);

	void teleportPlayer(GameObject* obj, LevelBehaviour* behaviour);

	GLFWwindow* getWindow();

	Shader font_shader;
	Camera camera;

	// GUI.
	std::map<int, std::vector<GUIObject*>> gui_objects;

	// Game Data.
	GameLevel* base_level = NULL;
	GameLevel* secret_level = NULL;
	GameLevel* pCurrentLevel = NULL;
	std::vector<LevelBehaviour*> levelBehaviours;

	int currentPage = PageType::MainMenu;

	int screenWidth, screenHeight;

	bool useMultisampling = false;
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

	int prevScore = 0;
	int prevLevel = 0;
	int merchPrices[2];

	ResourceManager* res_manager = NULL;
	SpriteRenderer renderer;

	GUIObject* pHealthBar = NULL;
	GUIObject* pEnergyBar = NULL;
	GUIObject* pEnemyBarFrame = NULL;
	GUIObject* pEnemyHealthBar = NULL;
	GUIObject* pRocketBars[3];
	GUIButton* pPauseButton = NULL;
	GUIButton* pResumeButton = NULL;
	GUITextBox* pScoreBox = NULL;
	GUITextBox* pFinalScore = NULL;
	GUITextBox* pLevelBox = NULL;
	GUITextBox* pCoinsBox = NULL;
	GUIObject* pActivateDeviceCaption = NULL;
	GUIObject* pIonFireCaption = NULL;

	std::vector<GUILayout*> dialogueTextLayouts;
};