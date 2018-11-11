#include "ImprovementBoxObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

ImprovementBoxObject::ImprovementBoxObject()
{
	damage = 0.0f;
	health = 1.0f;
	impulseFactor = 0;
	objectType = ObjectTypes::ImprovementBox;
}

ImprovementBoxObject::ImprovementBoxObject(bool createChildren)
{
}

ImprovementBoxObject::~ImprovementBoxObject()
{
	clear();
}

GameObject* ImprovementBoxObject::clone()
{
	ImprovementBoxObject* newObject = new ImprovementBoxObject(false);
	this->cloneParams(newObject);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);

	return newObject;
}

void ImprovementBoxObject::cloneParams(GameObject* obj)
{
	GameObject::cloneParams(obj);

	ImprovementBoxObject* box = (ImprovementBoxObject*)obj;
	box->setImprovement(this->improvement);
}

void ImprovementBoxObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void ImprovementBoxObject::update(float delta)
{
	GameObject::update(delta);
}

void ImprovementBoxObject::draw(bool useInstanced, int amount)
{
	GameObject::draw(useInstanced, amount);
}

void ImprovementBoxObject::resize()
{
	GameObject::resize();
}

void ImprovementBoxObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	GameObject::makeReaction(difference, otherObj, collisionChecker);

	if (!otherObj->isUsePhysics())
		return;

	SpacecraftObject* spacecraft = dynamic_cast<SpacecraftObject*>(otherObj);
	if (!spacecraft)
		return;

	improvement.improve(spacecraft);

	readyForDeath = true;
}

void ImprovementBoxObject::setImprovement(ImprovementStruct improv)
{
	improvement = improv;
}

ImprovementStruct ImprovementBoxObject::getImprovement()
{
	return improvement;
}

void ImprovementBoxObject::clear()
{
}

void ImprovementStruct::improve(SpacecraftObject* spacecraft)
{
	if (useHealth)
		spacecraft->setHealth(spacecraft->getHealth() + health);
	if (useEnergy)
		spacecraft->setMaxEnergy(spacecraft->getMaxEnergy() + energy);
	if (useRocketDetail)
		spacecraft->setRocketDetail(spacecraft->getRocketIntegrity() + rocketDetail);
}