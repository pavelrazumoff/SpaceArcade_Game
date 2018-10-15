#pragma once
#include "GameObject.h"

// GameLevel holds all Tiles as part of a Breakout level and 
// hosts functionality to Load/render levels from the harddisk.
class GameLevel
{
public:
	// Constructor
	GameLevel() { }

	void			init(Texture2D cubemap, Shader cubemapShader, SpriteRenderer* renderer);
	void			resize();
	// Render level
	void			Draw();

	void			handleInput(GLFWwindow *window, float delta);

	void			setScreenIndents(glm::vec4 indents);
	glm::vec4		getScreenIndents();

	SpriteRenderer* getRenderer();

	// Check if the level is completed (all non-solid tiles are destroyed)
	GLboolean		IsCompleted();
public:
	GameObject		spacecraft;

private:
	Texture2D		backgroundCubemap;
	Shader			cubemapShader;

	SpriteRenderer* renderer = NULL;
	glm::vec4		screenIndents;
};
