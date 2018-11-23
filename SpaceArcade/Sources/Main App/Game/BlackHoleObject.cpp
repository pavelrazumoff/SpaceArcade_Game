#include "BlackHoleObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

BlackHoleObject::BlackHoleObject()
{
	damage = 0.0f;
	usePhysics = false;
	objectType = ObjectTypes::BlackHole;
}

BlackHoleObject::BlackHoleObject(bool createChildren)
{
}

BlackHoleObject::~BlackHoleObject()
{
	clear();
}

GameObject* BlackHoleObject::clone()
{
	BlackHoleObject* newObject = new BlackHoleObject(false);
	this->cloneParams(newObject);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);

	return newObject;
}

void BlackHoleObject::cloneParams(GameObject* obj)
{
	GameObject::cloneParams(obj);

	BlackHoleObject* hole = (BlackHoleObject*)obj;
	hole->setBlackHoleType(this->getBlackHoleType());
}

void BlackHoleObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void BlackHoleObject::update(float delta)
{
	GameObject::update(delta);
}

void BlackHoleObject::draw(bool useInstanced, int amount)
{
	GameObject::draw(useInstanced, amount);
}

void BlackHoleObject::resize()
{
	GameObject::resize();

	glm::vec2 screenRatio = pLevel->getRenderer()->getScreenRatio();
	float ratio = Size.x / Size.y;

	Size.x = Size.x * screenRatio.x;
	Size.y = Size.x / ratio;
}

void BlackHoleObject::makeCollision(GameObject* obj)
{
	GameObject::makeCollision(obj);

	if (this->readyForDeath || obj->getReadyForDeath() ||
		!pLevel->getBehaviour()->checkForCollisionAddiction(this, obj))
		return;

	if (blackHoleType != BlackHoleType::Destroyer)
		return;

	glm::vec2 diff;
	this->checkCollision(obj, diff);

	if (glm::length(diff) < 20.0f)
		obj->setHealth(0.0f);
}

void BlackHoleObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	GameObject::makeReaction(difference, otherObj, collisionChecker);

	if (this->readyForDeath || otherObj->getReadyForDeath() ||
		!pLevel->getBehaviour()->checkForCollisionAddiction(this, otherObj))
		return;

	glm::vec2 normalizedDiff = glm::normalize(difference);
	if (!collisionChecker)
		normalizedDiff = -normalizedDiff;

	otherObj->applyImpulse(this->getImpulseFactor(), true);
	otherObj->Velocity = normalizedDiff * glm::vec2(this->getImpulseFactor());
}

void BlackHoleObject::setBlackHoleType(int type)
{
	blackHoleType = type;
}

int BlackHoleObject::getBlackHoleType()
{
	return blackHoleType;
}

void BlackHoleObject::clear()
{
	
}