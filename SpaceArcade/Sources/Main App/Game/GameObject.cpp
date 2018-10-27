#include "GameObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

GameObject::GameObject()
	: Position(0, 0), Size(1, 1), Velocity(0.0f), InitialRotation(0.0f), Rotation(0.0f)
{ 

}

GameObject::GameObject(bool createChildren)
	: Position(0, 0), Size(1, 1), Velocity(0.0f), InitialRotation(0.0f), Rotation(0.0f)
{
	
}

GameObject::~GameObject()
{
	clear();
}

GameObject* GameObject::clone()
{
	GameObject* newObject = new GameObject();
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);
	this->cloneParams(newObject);

	return newObject;
}

void GameObject::cloneParams(GameObject* obj)
{
	obj->InitialRotation = this->InitialRotation;
	obj->Rotation = this->Rotation;
	obj->setObjectType(this->getObjectType());
	obj->setIsDamagingObject(this->isDamagingObject());
	obj->setDamage(this->getDamage());
	obj->setHealth(this->getHealth());
	obj->setInitialHealth(this->getInitialHealth());
	obj->setParentObject(this->getParentObject());
	obj->setExplosionTime(this->explosionTime);
	obj->setExplosionSprite(this->ExplosionSprite);
	obj->setNonPlayerObject(this->nonPlayerObject);
	obj->setUsePhysics(this->isUsePhysics());
	obj->setAIController(this->getAIController());
	obj->setControlVelocityByRotation(this->isControlVelocityByRotation());
}

void GameObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity, bool addToLevel)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Velocity = velocity;
	this->pLevel = level;

	if(addToLevel)
		level->addNewObject(this);
}

void GameObject::update(float delta)
{
	if (health <= 0.0f)
	{
		if (explosionTime > 0.0f)
		{
			explosionTime -= delta;
			currentExplosionFrame = ExplosionSprite->numOfFrames - explosionTime / explosionTimeStep;
		}
		if (explosionTime <= 0.0f)
			readyForDeath = true;
	}

	if (health > 0.0f)
	{
		InitialRotation += Rotation * delta;

		glm::vec2 currentVelocity = Velocity;

		if (controlVelocityByRot)
		{
			glm::mat4 model;
			model = glm::rotate(model, glm::radians(InitialRotation), glm::vec3(0.0f, 0.0f, 1.0f));
			currentVelocity = model * glm::vec4(currentVelocity, 0.0f, 1.0f);
		}

		Position.x += currentVelocity.x * delta;
		Position.y += currentVelocity.y * delta;

		if (InitialRotation >= 360.0f || InitialRotation <= -360.0f)
			InitialRotation = 0.0f;

		if (!nonPlayerObject)
		{
			float restrZone = pLevel->getRenderer()->getCurrentScreenDimensions().y - pLevel->getPlayerRestrictionHeight();
			if (Position.y < restrZone)
				Position.y = restrZone;
		}
		else
		{
			// when object goes out of screen bounds when it already overstepped top side of the screen, destroy it.
			glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
			if (Position.y > 0.0f && (Position.x < 0.0f || Position.x > screenDimensions.x || Position.y > screenDimensions.y))
				setHealth(0.0f);
		}
	}

	// Update AI after all.
	if(aiController)
		aiController->update(delta);
}

void GameObject::Draw()
{
	if(visible && health > 0.0f)
		pLevel->getRenderer()->DrawSprite(this->Sprite, this->Position, this->Size, glm::vec4(0.0f), this->InitialRotation);
	if(health <= 0.0f && explosionTime > 0.0f && ExplosionSprite->ID >= 0)
		pLevel->getRenderer()->DrawSprite(this->ExplosionSprite, this->Position, this->Size, glm::vec4(0.0f), this->InitialRotation, currentExplosionFrame);
}

void GameObject::resize()
{
	glm::vec2 screenRatio = pLevel->getRenderer()->getScreenRatio();

	Position = glm::vec2(Position.x * screenRatio.x, Position.y * screenRatio.y);
}

void GameObject::handleInput(GLFWwindow *window, float delta)
{
}

void GameObject::processKey(int key, int action, bool* key_pressed)
{
}

bool GameObject::checkCollision(GameObject* obj, glm::vec2& difference)
{
	if (this->parentObject == obj || obj->getParentObject() == this)
		return false;

	// Collision x-axis?
	bool collisionX = this->Position.x + this->Size.x >= obj->Position.x &&
		obj->Position.x + obj->Size.x >= this->Position.x;
	// Collision y-axis?
	bool collisionY = this->Position.y + this->Size.y >= obj->Position.y &&
		obj->Position.y + obj->Size.y >= this->Position.y;

	glm::vec2 center(this->Position + glm::vec2(Size.x / 2, Size.y / 2));
	glm::vec2 center2(obj->Position + glm::vec2(obj->Size.x / 2, obj->Size.y / 2));
	// Get difference vector between both centers
	difference = center - center2;

	// Collision only if on both axes
	return collisionX && collisionY;
}

void GameObject::makeCollision(GameObject* obj)
{
	if (this->readyForDeath || obj->getReadyForDeath() ||
		!pLevel->getBehaviour()->checkForCollisionAddiction(this, obj))
		return;

	this->health -= obj->getDamage();
	obj->setHealth(obj->getHealth() - this->damage);

	if (healthChanged)
		healthChanged(this->health, this->initialHealth);
}

void GameObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	if (health <= 0.0f && explosionTime <= 0.0f)
	{
		readyForDeath = true;
		return;
	}
}

bool GameObject::isOffTheScreen(glm::vec2 screenDimensions)
{
	if (Position.x + Size.x < 0.0f || Position.y + Size.y < 0.0f ||
		Position.x > screenDimensions.x || Position.y > screenDimensions.y)
		return true;
	return false;
}

void GameObject::setParentObject(GameObject* parent)
{
	parentObject = parent;
}

GameObject* GameObject::getParentObject()
{
	return parentObject;
}

void GameObject::notify(GameObject* notifiedObject, NotifyCode code)
{
}

void GameObject::setExplosionSprite(Texture2D* sprite)
{
	ExplosionSprite = sprite;
	if (sprite)
		explosionTimeStep = explosionTime / sprite->numOfFrames;
}

void GameObject::setHealthChangedCallback(void(*actionCallback)(float, float))
{
	healthChanged = actionCallback;
}

void GameObject::setObjectType(int type)
{
	objectType = type;
}

void GameObject::setVisible(bool visible)
{
	this->visible = visible;
}

void GameObject::hideFromLevel(bool hide)
{
	hidden = hide;
}

void GameObject::setIsDamagingObject(bool damaging)
{
	damagingObject = damaging;
}

void GameObject::setDamage(float damage)
{
	this->damage = damage;
}

void GameObject::setHealth(float hp)
{
	health = hp;
}

void GameObject::setInitialHealth(float hp)
{
	initialHealth = hp;
}

void GameObject::setExplosionTime(float time)
{
	explosionTime = time;
}

void GameObject::setNonPlayerObject(bool nonPlayer)
{
	nonPlayerObject = nonPlayer;
}

void GameObject::setUsePhysics(bool physics)
{
	usePhysics = physics;
}

void GameObject::setAIController(AIController* controller)
{
	aiController = controller;
	if(controller)
		controller->setPawn(this);
}

void GameObject::setControlVelocityByRotation(bool control)
{
	controlVelocityByRot = control;
}

GameLevel* GameObject::getLevel()
{
	return pLevel;
}

int GameObject::getObjectType()
{
	return objectType;
}

bool GameObject::isVisible()
{
	return visible;
}

bool GameObject::isHiddenFromLevel()
{
	return hidden;
}

bool GameObject::isDamagingObject()
{
	return damagingObject;
}

float GameObject::getDamage()
{
	return damage;
}

float GameObject::getHealth()
{
	return health;
}

float GameObject::getInitialHealth()
{
	return initialHealth;
}

bool GameObject::getReadyForDeath()
{
	return readyForDeath;
}

float GameObject::getExplosionTime()
{
	return explosionTime;
}

bool GameObject::isNonPlayerObject()
{
	return nonPlayerObject;
}

bool GameObject::isUsePhysics()
{
	return usePhysics;
}

AIController* GameObject::getAIController()
{
	return aiController;
}

bool GameObject::isControlVelocityByRotation()
{
	return controlVelocityByRot;
}

void GameObject::clear()
{
	
}