/*
	* Space Arcade Game
	* Copyright (C) 2018 Paul Razumov.
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	* Source:
	*  https://github.com/pavelrazumoff/SpaceArcade_Game
*/

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
	if (!spacecraft || spacecraft->getAIController())
		return;

	improvement.improve(spacecraft);

	if (smokeObject)
	{
		smokeObject->Position = this->Position - glm::vec2(smokeObject->Size.x / 2, smokeObject->Size.y / 2);
		smokeObject->Velocity = glm::vec2(this->Velocity.x / 1.5f, this->Velocity.y / 1.5f);
		smokeObject->hideFromLevel(false);
		smokeObject->startSelfDestroying(true);
		smokeObject = NULL;
	}

	readyForDeath = true;
}

void ImprovementBoxObject::setImprovement(ImprovementStruct improv)
{
	improvement = improv;
}

void ImprovementBoxObject::setSmokeObject(GameObject* smoke)
{
	smokeObject = smoke;
	if(smokeObject)
		smokeObject->hideFromLevel(true);
}

ImprovementStruct ImprovementBoxObject::getImprovement()
{
	return improvement;
}

GameObject* ImprovementBoxObject::getSmokeObject()
{
	return smokeObject;
}

void ImprovementBoxObject::clear()
{
	if (smokeObject)
	{
		pLevel->removeObject(smokeObject);
		delete smokeObject;
		smokeObject = NULL;
	}
}

void ImprovementStruct::improve(SpacecraftObject* spacecraft)
{
	if (useHealth)
		spacecraft->setHealth(spacecraft->getHealth() + health);
	if (useEnergy)
		spacecraft->setMaxEnergy(spacecraft->getMaxEnergy() + energy);
	if (useRocketDetail)
		spacecraft->setRocketDetail(spacecraft->getRocketIntegrity() + rocketDetail);
	if (useBlackHole)
		spacecraft->setBlackHolePortal(blackHole);
	if (useCoinValue)
		spacecraft->setCoins(spacecraft->getCoins() + coinValue);
}