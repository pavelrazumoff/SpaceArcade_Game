#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

// Container object for holding all state relevant for a single
// game object entity.
class GameLevel;

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
	virtual void resize();
	virtual void clear();

	// Draw sprite
	virtual void Draw();
	virtual void update(float delta);

	void setVisible(bool visible);

	// Object state
	glm::vec2   Position, Size, Velocity;
	GLfloat     Rotation;
	// Render state
	Texture2D   Sprite;

protected:
	GameLevel* pLevel = NULL;
	bool visible = true;
};