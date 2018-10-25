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

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f), bool addToLevel = true);
	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	// Draw sprite
	virtual void Draw();
	virtual void update(float delta);

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action, bool* key_pressed);

	void setParentObject(GameObject* parent);
	GameObject* getParentObject();

	virtual void notify(GameObject* notifiedObject, NotifyCode code);

	void setExplosionSprite(Texture2D* sprite);
	void setHealthChangedCallback(void(*actionCallback)(float, float));

	void setObjectType(int type);
	void setVisible(bool visible);
	void setIsDamagingObject(bool damaging);
	void setDamage(float damage);
	void setHealth(float hp);
	void setInitialHealth(float hp);
	void setExplosionTime(float time);
	void setUsePhysics(bool physics);
	void setUseAI(bool useAI);
	virtual void setControlVelocityByRotation(bool control);

	GameLevel* getLevel();
	int getObjectType();
	bool isVisible();
	bool isDamagingObject();
	float getDamage();
	float getHealth();
	float getInitialHealth();
	float getExplosionTime();
	bool isUsePhysics();
	bool isAIControlled();
	bool isControlVelocityByRotation();

	bool isOffTheScreen(glm::vec2 screenDimensions);
	bool getReadyForDeath();

	virtual bool checkCollision(GameObject* obj, glm::vec2& difference);

	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	// Object state
	glm::vec2   Position, Size, Velocity;
	glm::vec2	VelocityScale;
	GLfloat     InitialRotation, Rotation;
	// Render state
	Texture2D*   Sprite = NULL;
	Texture2D*	ExplosionSprite = NULL;

protected:
	GameLevel* pLevel = NULL;
	GameObject* parentObject = NULL;

	int objectType = -1;
	bool visible = true;
	bool damagingObject = true;
	float damage = 1.0f;
	float health = 100.0f;
	float initialHealth = 100.0f;
	float explosionTime = 0.0f;
	bool usePhysics = false;
	bool controlledByAI = true;
	bool controlVelocityByRot = false;

	int currentExplosionFrame = 0;
	float explosionTimeStep = 0.0f;
	bool readyForDeath = false;

	float impulseFactor = 200;

	void(*healthChanged)(float, float) = NULL;
};