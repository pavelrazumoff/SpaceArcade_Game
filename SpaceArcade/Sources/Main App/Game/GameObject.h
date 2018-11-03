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
	Basic,
	LaserRay,
	Meteorite,
	SpaceCraft,
	EnergyBarrier,
	ElectricShock
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

	void setLevel(GameLevel* level);
	void setObjectType(int type);
	void setSprite(Texture2D* sprite);
	void setVisible(bool visible);
	virtual void hideFromLevel(bool hide);
	void setIsDamagingObject(bool damaging);
	void setDamage(float damage);
	void setHealth(float hp);
	void setInitialHealth(float hp);
	void setReadyForDeath(bool ready);
	void setExplosionTime(float time);
	void setSelfDestroyTime(float time);
	void startSelfDestroying(bool start);
	void setNonPlayerObject(bool nonPlayer);
	void setUsePhysics(bool physics);
	void setAIController(AIController* controller);
	virtual void setControlVelocityByRotation(bool control);
	void setDamageAsAttachment(bool damage);
	void setUseAnimation(bool animation);
	void setAnimationDuration(float duration);

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
	bool isDamageAsAttachment();
	bool isUseAnimation();
	float getAnimationDuration();

	bool isOffTheScreen(glm::vec2 screenDimensions);
	bool getReadyForDeath();

	virtual bool checkCollision(GameObject* obj, glm::vec2& difference);

	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void attachNewObject(GameObject* obj);
	void removeAttachedObject(GameObject* obj);

	int getAttachedObjectsSize();
	GameObject* getAttachedObjectByIndex(int index);
	bool isObjectAttachedTo(GameObject* obj);

	// Object state
	glm::vec2   Position, Size, Velocity;
	glm::vec2	RelativePosition;
	glm::vec2	VelocityScale;
	GLfloat     InitialRotation, Rotation;
	// Render state
	Texture2D*  Sprite = NULL;
	Texture2D*	ExplosionSprite = NULL;

protected:
	GameLevel* pLevel = NULL;
	GameObject* parentObject = NULL;
	AIController* aiController = NULL;

	std::vector<GameObject*> attachedObjects;

	glm::mat4 model;

	int objectType = ObjectTypes::None;
	bool visible = true;
	bool hidden = false;
	bool damagingObject = true;
	float damage = 1.0f;
	float health = 100.0f;
	float initialHealth = 100.0f;
	float explosionTime = 0.0f;
	float selfDestroyTime = 0.0f;
	bool selfDestroying = false;
	bool nonPlayerObject = true;
	bool usePhysics = false;
	bool controlVelocityByRot = false;
	bool damageAsAttachment = false;
	bool useAnimation = false;

	int currentAnimationFrame = 0;
	float animationDuration = 0.0f;
	float animationTime = 0.0f;
	float animationTimeStep = 0.0f;

	int currentExplosionFrame = 0;
	float explosionTimeStep = 0.0f;
	bool readyForDeath = false;

	float impulseFactor = 200;

	void(*healthChanged)(float, float) = NULL;
};