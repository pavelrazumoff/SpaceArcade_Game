#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

// Container object for holding all state relevant for a single
// game object entity.
class GameLevel;
class AIController;

enum NotifyCode
{
	Destroyed = 0,
};

enum ObjectTypes
{
	None = 0,
	LaserRay,
	Meteorite,
	SpaceCraft
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
	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);
	virtual void updateModelMatrix();

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action, bool* key_pressed);

	void setParentObject(GameObject* parent);
	GameObject* getParentObject();

	virtual void notify(GameObject* notifiedObject, NotifyCode code);

	void setExplosionSprite(Texture2D* sprite);
	void setHealthChangedCallback(void(*actionCallback)(float, float));

	void setObjectType(int type);
	void setVisible(bool visible);
	void hideFromLevel(bool hide);
	void setIsDamagingObject(bool damaging);
	void setDamage(float damage);
	void setHealth(float hp);
	void setInitialHealth(float hp);
	void setExplosionTime(float time);
	void setNonPlayerObject(bool nonPlayer);
	void setUsePhysics(bool physics);
	void setAIController(AIController* controller);
	virtual void setControlVelocityByRotation(bool control);

	GameLevel* getLevel();
	glm::mat4 getModel();
	int getObjectType();
	bool isVisible();
	bool isHiddenFromLevel();
	bool isDamagingObject();
	float getDamage();
	float getHealth();
	float getInitialHealth();
	float getExplosionTime();
	bool isNonPlayerObject();
	bool isUsePhysics();
	AIController* getAIController();
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
	AIController* aiController = NULL;

	glm::mat4 model;

	int objectType = ObjectTypes::None;
	bool visible = true;
	bool hidden = false;
	bool damagingObject = true;
	float damage = 1.0f;
	float health = 100.0f;
	float initialHealth = 100.0f;
	float explosionTime = 0.0f;
	bool nonPlayerObject = true;
	bool usePhysics = false;
	bool controlVelocityByRot = false;

	int currentExplosionFrame = 0;
	float explosionTimeStep = 0.0f;
	bool readyForDeath = false;

	float impulseFactor = 200;

	void(*healthChanged)(float, float) = NULL;
};