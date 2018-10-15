#pragma once
#include "GameObject.h"

// GameLevel holds all Tiles as part of a Breakout level and 
// hosts functionality to Load/render levels from the harddisk.
class GameLevel
{
public:
	// Constructor
	GameLevel() { }

	void      init(Texture2D cubemap, Shader cubemapShader);
	// Render level
	void      Draw(SpriteRenderer &renderer);

	void	  handleInput(GLFWwindow *window, float delta);

	// Check if the level is completed (all non-solid tiles are destroyed)
	GLboolean IsCompleted();
public:
	GameObject spacecraft;

private:
	Texture2D backgroundCubemap;
	Shader cubemapShader;
};
