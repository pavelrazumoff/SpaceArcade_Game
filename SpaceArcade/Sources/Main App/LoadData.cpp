#include "MainApp.h"

void MainApp::loadShaders()
{
	res_manager->LoadShader("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//ScreenShaderFS.glsl", "Screen");
	res_manager->LoadShader("Shaders//Skybox//SkyboxShaderVS.glsl", "Shaders//Skybox//SkyboxShaderFS.glsl", "Skybox");
	res_manager->LoadShader("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//FinalShaderFS.glsl", "Final");
	res_manager->LoadShader("Shaders//Blur//BlurShaderVS.glsl", "Shaders//Blur//BlurShaderFS.glsl", "Blur");
	res_manager->LoadShader("Shaders//Sprite//SpriteShaderVS.glsl", "Shaders//Sprite//SpriteShaderFS.glsl", "Sprite");

	//textures.
	// use this params for non-resized objects (not maximized mostly).
	// this will clean all borders artifacts of textures.
	TextureParams gameObjectsParams;
	gameObjectsParams.Filter_Min = GL_LINEAR_MIPMAP_NEAREST;
	gameObjectsParams.Filter_Max = GL_NEAREST;

	res_manager->LoadTexture("Images//SpaceShip.png", "spacecraft", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//SpaceShipEnemy.png", "spacecraftEnemy", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//SpaceShipBoss.png", "spacecraftBoss", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//SpaceShipBoss2.png", "spacecraftBoss2", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//roadster.png", "roadster", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//starmanSpace.png", "starman", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//laser_ray.png", "laserRayBlue", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//laser_rayEnemy.png", "laserRayRed", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//asteroid.png", "asteroid", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//explosion.png", "explosion", useGammaCorrection);
	res_manager->LoadTexture("Images//energy_barrier.png", "energyBarrier", useGammaCorrection);
	res_manager->LoadTexture("Images//generatorLeft.png", "leftGenerator", useGammaCorrection);
	res_manager->LoadTexture("Images//generatorRight.png", "rightGenerator", useGammaCorrection);
	res_manager->LoadTexture("Images//electric_shock.png", "electricShock", useGammaCorrection);
	res_manager->LoadTexture("Images//blast_wave.png", "blastWave", useGammaCorrection);
	res_manager->LoadTexture("Images//health_kit.png", "healthKit", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//energy_shield.png", "energyShield", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//rocket.png", "rocket", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//smoke.png", "smoke", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//space_station.png", "spaceStation", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//ionWeapon.png", "ionWeapon", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//ion_charge.png", "ionCharge", useGammaCorrection, &gameObjectsParams);
	res_manager->LoadTexture("Images//coin.png", "coin", useGammaCorrection, &gameObjectsParams);

	for (int i = 0; i < 4; ++i)
	{
		std::string path = "Images//black_hole" + std::to_string(i + 1) + ".png";
		std::string name = "blackHole" + std::to_string(i + 1);
		res_manager->LoadTexture(path.c_str(), name, useGammaCorrection, &gameObjectsParams);
	}

	for (int i = 0; i < 6; ++i)
	{
		std::string path = "Images//debris1_" + std::to_string(i) + ".png";
		std::string name = "debris1_" + std::to_string(i);
		res_manager->LoadTexture(path.c_str(), name, useGammaCorrection, &gameObjectsParams);
	}

	for (int i = 0; i < 3; ++i)
	{
		std::string path = "Images//rocket_detail1_" + std::to_string(i) + ".png";
		std::string name = "rocketDetail1_" + std::to_string(i);
		res_manager->LoadTexture(path.c_str(), name, useGammaCorrection);
	}

	Texture2D* tex = res_manager->GetTexture("explosion");
	tex->numOfColumns = 8;
	tex->numOfRows = 4;
	tex->numOfFrames = 32;

	tex = res_manager->GetTexture("energyBarrier");
	tex->numOfColumns = 1;
	tex->numOfRows = 6;
	tex->numOfFrames = 6;

	tex = res_manager->GetTexture("electricShock");
	tex->numOfColumns = 5;
	tex->numOfRows = 2;
	tex->numOfFrames = 8;

	tex = res_manager->GetTexture("blastWave");
	tex->numOfColumns = 6;
	tex->numOfRows = 2;
	tex->numOfFrames = 11;

	tex = res_manager->GetTexture("energyShield");
	tex->numOfColumns = 1;
	tex->numOfRows = 5;
	tex->numOfFrames = 5;

	tex = res_manager->GetTexture("smoke");
	tex->numOfColumns = 8;
	tex->numOfRows = 8;
	tex->numOfFrames = 64;

	// sounds.
	res_manager->addSound("Sounds//sound1_0.mp3", "BackgroundSound");
	res_manager->addSound("Sounds//sound1_1.mp3", "BackgroundSound2");
	res_manager->addSound("Sounds//laser_effect.mp3", "LaserSound");
	res_manager->addSound("Sounds//rocketFire_effect.wav", "RocketSound");
	res_manager->addSound("Sounds//laser_effectEnemy.mp3", "LaserEnemySound");
	res_manager->addSound("Sounds//ionCharge_effect.wav", "IonChargeEffect");
	res_manager->addSound("Sounds//explosion_effect1_0.mp3", "ExplosionEffect");
	res_manager->addSound("Sounds//explosion_effect1_1.mp3", "ExplosionEffect2");
	res_manager->addSound("Sounds//electricExplosion_effect.mp3", "ElectricExplosionEffect");
	res_manager->addSound("Sounds//generator_effect.wav", "GeneratorEffect");
	res_manager->addSound("Sounds//click_effect.wav", "ClickEffect");

	// gui.
	res_manager->LoadTexture("Images//Interface//gameCaption.png", "gameCaption", false);

	res_manager->LoadTexture("Images//Interface//playGameButton.png", "playGameButton", false);
	res_manager->LoadTexture("Images//Interface//playGameButtonHovered.png", "playGameButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//playGameButtonPressed.png", "playGameButtonPressed", false);

	res_manager->LoadTexture("Images//Interface//settingsButton.png", "settingsButton", false);
	res_manager->LoadTexture("Images//Interface//settingsButtonHovered.png", "settingsButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//settingsButtonPressed.png", "settingsButtonPressed", false);

	res_manager->LoadTexture("Images//Interface//quitButton.png", "quitButton", false);
	res_manager->LoadTexture("Images//Interface//quitButtonHovered.png", "quitButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//quitButtonPressed.png", "quitButtonPressed", false);

	res_manager->LoadTexture("Images//Interface//backButton.png", "backButton", false);
	res_manager->LoadTexture("Images//Interface//backButtonHovered.png", "backButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//backButtonPressed.png", "backButtonPressed", false);

	res_manager->LoadTexture("Images//Interface//settingsPageFrame.png", "settingsFrame", false);
	res_manager->LoadTexture("Images//Interface//checkBox.png", "checkBox", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//checkBoxHovered.png", "checkBoxHovered", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//checkBoxPressed.png", "checkBoxPressed", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//checkIcon.png", "checkIcon", false, &gameObjectsParams);

	res_manager->LoadTexture("Images//Interface//gameOverCaption.png", "gameOverCaption", false);
	res_manager->LoadTexture("Images//Interface//playAgainButton.png", "playAgainButton", false);
	res_manager->LoadTexture("Images//Interface//playAgainButtonHovered.png", "playAgainButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//playAgainButtonPressed.png", "playAgainButtonPressed", false);

	res_manager->LoadTexture("Images//Interface//healthBar.png", "healthBar", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//energyBar.png", "energyBar", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//enemyHealthBar.png", "enemyHealthBar", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//rocketContent.png", "rocketContent", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//rocketContentFilled.png", "rocketFilledContent", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//barFrame.png", "barFrame", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//enemyBarFrame.png", "enemyBarFrame", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//rocketFrame.png", "rocketFrame", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//healthCaption.png", "healthCaption", false);
	res_manager->LoadTexture("Images//Interface//energyCaption.png", "energyCaption", false);
	res_manager->LoadTexture("Images//Interface//rocketFireCaption.png", "rocketFireCaption", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//rocketCreateCaption.png", "rocketCreateCaption", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//deviceActivateCaption.png", "deviceActivateCaption", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//ionFireCaption.png", "ionFireCaption", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//pauseButton.png", "pauseButton", false);
	res_manager->LoadTexture("Images//Interface//pauseButtonHovered.png", "pauseButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//pauseButtonPressed.png", "pauseButtonPressed", false);
	res_manager->LoadTexture("Images//Interface//resumeButton.png", "resumeButton", false);
	res_manager->LoadTexture("Images//Interface//resumeButtonHovered.png", "resumeButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//resumeButtonPressed.png", "resumeButtonPressed", false);
	res_manager->LoadTexture("Images//Interface//creditsButton.png", "creditsButton", false);
	res_manager->LoadTexture("Images//Interface//creditsButtonHovered.png", "creditsButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//creditsButtonPressed.png", "creditsButtonPressed", false);

	res_manager->LoadTexture("Images//Interface//dialogueInterface.png", "dialogueInterface", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//starmanIcon.png", "starmanIcon", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//dialogueButton.png", "dialogueButton", false);
	res_manager->LoadTexture("Images//Interface//dialogueButtonHovered.png", "dialogueButtonHovered", false);
	res_manager->LoadTexture("Images//Interface//dialogueButtonPressed.png", "dialogueButtonPressed", false);
	res_manager->LoadTexture("Images//Interface//ionWeaponButton.png", "ionWeaponButton", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//ionWeaponButtonHovered.png", "ionWeaponButtonHovered", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//ionWeaponButtonPressed.png", "ionWeaponButtonPressed", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//healthFillButton.png", "healthFillButton", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//healthFillButtonHovered.png", "healthFillButtonHovered", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//healthFillButtonPressed.png", "healthFillButtonPressed", false, &gameObjectsParams);
	res_manager->LoadTexture("Images//Interface//coinIcon.png", "coinIcon", false, &gameObjectsParams);

	//font.
	font_shader.load("Shaders//Font//FontShaderVS.glsl", "Shaders//Font//FontShaderFS.glsl");
}

void MainApp::loadTextures()
{
	std::vector<const GLchar*> facesBlue = {
		"Images//Skybox//blue//bkg1_right.png",
		"Images//Skybox//blue//bkg1_left.png",
		"Images//Skybox//blue//bkg1_top.png",
		"Images//Skybox//blue//bkg1_bot.png",
		"Images//Skybox//blue//bkg1_front.png",
		"Images//Skybox//blue//bkg1_back.png"
	};

	res_manager->LoadCubemap(facesBlue, "BlueSpace", useGammaCorrection);

	std::vector<const GLchar*> facesNebula = {
		"Images//Skybox//nebula//bkg1_right.png",
		"Images//Skybox//nebula//bkg1_left.png",
		"Images//Skybox//nebula//bkg1_top.png",
		"Images//Skybox//nebula//bkg1_bot.png",
		"Images//Skybox//nebula//bkg1_front.png",
		"Images//Skybox//nebula//bkg1_back.png"
	};

	res_manager->LoadCubemap(facesNebula, "NebulaSpace", useGammaCorrection);
}