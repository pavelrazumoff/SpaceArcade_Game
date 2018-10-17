#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

// Container object for holding all state relevant for a single
// game object entity.
class GameLevel;

enum NotifyCode
{
	Destroyed = 0,
};

class GameObject
{
public:
	GameObject();
	GameObject(bool createChildren);
	virtual ~GameObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f), bool addToLevel = true);
	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();

	// Draw sprite
	virtual void Draw();
	virtual void update(float delta);

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action, bool* key_pressed);

	void setParentObject(GameObject* parent);
	GameObject* getParentObject();

	virtual void notify(GameObject* notifiedObject, NotifyCode code);

	void setVisible(bool visible);
	void setIsDamagingObject(bool damaging);
	void setDamage(float damage);
	void setHealth(float hp);

	bool isVisible();
	bool isDamagingObject();
	float getDamage();
	float getHealth();

	bool getReadyForDeath();

	virtual bool checkCollision(GameObject* obj);
	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction();

	// Object state
	glm::vec2   Position, Size, Velocity;
	GLfloat     Rotation;
	// Render state
	Texture2D   Sprite;

protected:
	GameLevel* pLevel = NULL;
	GameObject* parentObject = NULL;

	bool visible = true;
	bool damagingObject = true;
	float damage = 1.0f;
	float health = 100.0f;

	bool readyForDeath = false;
};