#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

// Container object for holding all state relevant for a single
// game object entity.
class GameLevel;
class AIController;

enum NotifyCode
{
	Destroyed = 0,
	BlastFinished
};

enum ObjectTypes
{
	None = 0,
	Basic,
	LaserRay,
	Meteorite,
	SpaceCraft,
	BossSpaceCraft,
	EnergyBarrier,
	ElectricShock,
	EnergyShield,
	BlastWave,
	ImprovementBox,
	Rocket,
	Debris,
	BlackHole,
	SpaceStation
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
	virtual void updatePaused(float delta);
	virtual void updateModelMatrix();
	virtual void updateAnimation(float delta);

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action, bool* key_pressed);

	void setParentObject(GameObject* parent);
	GameObject* getParentObject();

	virtual bool notify(GameObject* notifiedObject, NotifyCode code);

	void setExplosionSprite(Texture2D* sprite);
	void setHealthChangedCallback(void(*actionCallback)(float, float));
	void setExplosionSoundName(std::string name);

	void setLevel(GameLevel* level);
	void setObjectType(int type);
	void setSprite(Texture2D* sprite);
	void setVisible(bool visible);
	virtual void hideFromLevel(bool hide);
	void setIsDamagingObject(bool damaging);
	void setDamage(float damage);
	void setHealth(float hp);
	void setMaxHealth(float hp);
	void setReadyForDeath(bool ready);
	void setExplosionTime(float time);
	void setSelfDestroyTime(float time);
	virtual void startSelfDestroying(bool start);
	void setNonPlayerObject(bool nonPlayer);
	void setUsePhysics(bool physics);
	void setCollisionCheck(bool check);
	void setAIController(AIController* controller);
	virtual void setControlVelocityByRotation(bool control);
	void setDamageAsAttachment(bool damage);
	void setUseAnimation(bool animation);
	void setUseBackAndForthAnimation(bool backForth);
	void setAnimationOrder(bool order);
	void setAnimationDuration(float duration);
	void setImpulseFactor(float impulse);
	void applyImpulse(float impulse, bool free = false);
	void setScoreContribution(int score);

	GameLevel* getLevel();
	glm::mat4 getModel();
	int getObjectType();
	bool isVisible();
	bool isHiddenFromLevel();
	bool isDamagingObject();
	float getDamage();
	float getHealth();
	float getMaxHealth();
	float getExplosionTime();
	bool isNonPlayerObject();
	bool isUsePhysics();
	bool isCheckingCollision();
	AIController* getAIController();
	bool isControlVelocityByRotation();
	bool isDamageAsAttachment();
	bool isUseAnimation();
	bool isUseBackAndForthAnimation();
	bool getAnimationOrder();
	float getAnimationDuration();
	float getImpulseFactor();
	int getScoreContribution();

	bool isOffTheScreen(glm::vec2 screenDimensions);
	bool getReadyForDeath();

	virtual bool checkCollision(GameObject* obj, glm::vec2& difference);

	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void attachNewObject(GameObject* obj, bool onTop = true);
	void removeAttachedObject(GameObject* obj);
	void addPostDeathObject(GameObject* obj);

	int getAttachedObjectsSize();
	int getAttachedObjectsSizeByType(int type);
	int getPostDeathObjectsSize();
	GameObject* getAttachedObjectByIndex(int index);
	GameObject* getAttachedObjectByTypeIndex(int type, int index);
	GameObject* getPostDeathObjectByIndex(int index);
	bool isObjectAttachedTo(GameObject* obj);

	// Object state
	glm::vec2   Position, Size, Velocity;
	glm::vec2	RelativePosition;
	glm::vec2	VelocityScale;
	GLfloat     InitialRotation, Rotation;

	// Render state
	Texture2D*  Sprite = NULL;
	Texture2D*	ExplosionSprite = NULL;
	glm::vec4	color = glm::vec4(1.0f);
	glm::vec4	explosionColor = glm::vec4(1.0f);

protected:
	GameLevel* pLevel = NULL;
	GameObject* parentObject = NULL;
	AIController* aiController = NULL;

	std::vector<GameObject*> attachedObjects;
	std::vector<GameObject*> attachedOnTop;
	std::vector<GameObject*> attachedOnBottom;
	std::vector<GameObject*> postDeathObjects;		// spawned after death of this object (when readyForDeath = true).

	glm::mat4 model;
	glm::mat4 explosionModel;

	int objectType = ObjectTypes::None;
	bool visible = true;
	bool hidden = false;
	bool damagingObject = true;
	float damage = 1.0f;
	float health = 100.0f;
	float maxHealth = 100.0f;
	float explosionTime = 0.0f;
	float selfDestroyTime = 0.0f;
	bool selfDestroying = false;
	bool nonPlayerObject = true;
	bool usePhysics = false;
	bool collisionCheck = true;
	bool controlVelocityByRot = false;
	bool damageAsAttachment = false;

	bool useAnimation = false;
	bool useBackAndForthAnim = false;
	bool animationOrder = true;				// true - forward, false - backward.
	int currentAnimationFrame = 0;
	float animationDuration = 0.0f;
	float animationTime = 0.0f;
	float animationTimeStep = 0.0f;

	int currentExplosionFrame = 0;
	float explosionTimeStep = 0.0f;
	bool readyForDeath = false;
	bool useExplosionSize = false;
	std::string explosionSoundName;

	float impulseFactor = 200;
	float appliedImpulse = 0;
	bool freeImpulse = false;

	int scoreContribution = 10;

	void(*healthChanged)(float, float) = NULL;
};