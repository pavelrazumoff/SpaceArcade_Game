#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameLevel;

class GameObject
{
public:
	// Constructor(s)
	GameObject();

	void init(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
	void resize();
	// Draw sprite
	virtual void Draw();

	void handleInput(GLFWwindow *window, float delta);

	void setLevelPointer(GameLevel* level);

	// Object state
	glm::vec2   Position, Size, Velocity;
	GLfloat     Rotation;
	// Render state
	Texture2D   Sprite;

private:
	GameLevel* pLevel = NULL;
};