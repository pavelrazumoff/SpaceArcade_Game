#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
	// Constructor(s)
	GameObject();

	void init(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
	// Draw sprite
	virtual void Draw(SpriteRenderer &renderer, glm::vec2 screenRatio);

	void handleInput(GLFWwindow *window, float delta);

	// Object state
	glm::vec2   SourcePosition, Size, Velocity;
	glm::vec2	Position;
	GLfloat     Rotation;
	// Render state
	Texture2D   Sprite;
};