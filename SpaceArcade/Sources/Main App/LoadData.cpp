#include "MainApp.h"

void MainApp::loadShaders()
{
	res_manager.LoadShader("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//ScreenShaderFS.glsl", "Screen");
	res_manager.LoadShader("Shaders//Skybox//SkyboxShaderVS.glsl", "Shaders//Skybox//SkyboxShaderFS.glsl", "Skybox");
	res_manager.LoadShader("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//FinalShaderFS.glsl", "Final");
	res_manager.LoadShader("Shaders//Blur//BlurShaderVS.glsl", "Shaders//Blur//BlurShaderFS.glsl", "Blur");
	res_manager.LoadShader("Shaders//Sprite//SpriteShaderVS.glsl", "Shaders//Sprite//SpriteShaderFS.glsl", "Sprite");

	//textures.
	// use this params for non-resized objects (not maximized mostly).
	// this will clean all borders artefacts of textures.
	TextureParams gameObjectsParams;
	gameObjectsParams.Filter_Min = GL_LINEAR_MIPMAP_NEAREST;
	gameObjectsParams.Filter_Max = GL_NEAREST;

	res_manager.LoadTexture("Images//SpaceShip.png", "spacecraft", useGammaCorrection, &gameObjectsParams);
	res_manager.LoadTexture("Images//SpaceShipEnemy.png", "spacecraftEnemy", useGammaCorrection, &gameObjectsParams);
	res_manager.LoadTexture("Images//laser_ray.png", "laserRayBlue", useGammaCorrection, &gameObjectsParams);
	res_manager.LoadTexture("Images//laser_rayEnemy.png", "laserRayRed", useGammaCorrection, &gameObjectsParams);
	res_manager.LoadTexture("Images//asteroid.png", "asteroid", useGammaCorrection, &gameObjectsParams);
	res_manager.LoadTexture("Images//explosion.png", "explosion", useGammaCorrection);

	// gui.
	res_manager.LoadTexture("Images//Interface//playGameButton.png", "playGameButton", false);
	res_manager.LoadTexture("Images//Interface//playGameButtonHovered.png", "playGameButtonHovered", false);
	res_manager.LoadTexture("Images//Interface//playGameButtonPressed.png", "playGameButtonPressed", false);

	res_manager.LoadTexture("Images//Interface//settingsButton.png", "settingsButton", false);
	res_manager.LoadTexture("Images//Interface//settingsButtonHovered.png", "settingsButtonHovered", false);
	res_manager.LoadTexture("Images//Interface//settingsButtonPressed.png", "settingsButtonPressed", false);

	res_manager.LoadTexture("Images//Interface//quitButton.png", "quitButton", false);
	res_manager.LoadTexture("Images//Interface//quitButtonHovered.png", "quitButtonHovered", false);
	res_manager.LoadTexture("Images//Interface//quitButtonPressed.png", "quitButtonPressed", false);

	res_manager.LoadTexture("Images//Interface//backButton.png", "backButton", false);
	res_manager.LoadTexture("Images//Interface//backButtonHovered.png", "backButtonHovered", false);
	res_manager.LoadTexture("Images//Interface//backButtonPressed.png", "backButtonPressed", false);

	res_manager.LoadTexture("Images//Interface//settingsPageFrame.png", "settingsFrame", false);
	res_manager.LoadTexture("Images//Interface//checkBox.png", "checkBox", false, &gameObjectsParams);
	res_manager.LoadTexture("Images//Interface//checkBoxHovered.png", "checkBoxHovered", false, &gameObjectsParams);
	res_manager.LoadTexture("Images//Interface//checkBoxPressed.png", "checkBoxPressed", false, &gameObjectsParams);
	res_manager.LoadTexture("Images//Interface//checkIcon.png", "checkIcon", false, &gameObjectsParams);

	res_manager.LoadTexture("Images//Interface//healthBar.png", "healthBar", false);
	res_manager.LoadTexture("Images//Interface//energyBar.png", "energyBar", false);
	res_manager.LoadTexture("Images//Interface//barFrame.png", "barFrame", false);
	res_manager.LoadTexture("Images//Interface//healthCaption.png", "healthCaption", false);
	res_manager.LoadTexture("Images//Interface//energyCaption.png", "energyCaption", false);

	Texture2D* tex = res_manager.GetTexture("explosion");
	tex->numOfColumns = 8;
	tex->numOfRows = 4;
	tex->numOfFrames = 32;

	//font.
	font_shader.load("Shaders//Font//FontShaderVS.glsl", "Shaders//Font//FontShaderFS.glsl");
}

void MainApp::loadTextures()
{
	std::vector<const GLchar*> faces = {
		"Images//Skybox//blue//bkg1_right.png",
		"Images//Skybox//blue//bkg1_left.png",
		"Images//Skybox//blue//bkg1_top.png",
		"Images//Skybox//blue//bkg1_bot.png",
		"Images//Skybox//blue//bkg1_front.png",
		"Images//Skybox//blue//bkg1_back.png"
	};

	res_manager.LoadCubemap(faces, "BlueSpace", useGammaCorrection);
}