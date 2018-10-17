#include "MainApp.h"

void MainApp::loadShaders()
{
	res_manager.LoadShader("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//ScreenShaderFS.glsl", "Screen");
	res_manager.LoadShader("Shaders//Skybox//SkyboxShaderVS.glsl", "Shaders//Skybox//SkyboxShaderFS.glsl", "Skybox");
	res_manager.LoadShader("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//FinalShaderFS.glsl", "Final");
	res_manager.LoadShader("Shaders//Blur//BlurShaderVS.glsl", "Shaders//Blur//BlurShaderFS.glsl", "Blur");
	res_manager.LoadShader("Shaders//Sprite//SpriteShaderVS.glsl", "Shaders//Sprite//SpriteShaderFS.glsl", "Sprite");

	//textures.
	res_manager.LoadTexture("Images//SpaceShip.png", "spacecraft", useGammaCorrection);
	res_manager.LoadTexture("Images//laser_ray.png", "laserRay", useGammaCorrection);
	res_manager.LoadTexture("Images//asteroid.png", "asteroid", useGammaCorrection);
	res_manager.LoadTexture("Images//explosion.png", "explosion", useGammaCorrection);

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