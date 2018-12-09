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

#include "EnergyBarrierObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

EnergyBarrierObject::EnergyBarrierObject()
{
	damage = 0.1f;
	usePhysics = false;
	useAnimation = true;
	objectType = ObjectTypes::EnergyBarrier;

	for (int i = 0; i < 2; ++i)
		generators[i] = NULL;
}

EnergyBarrierObject::EnergyBarrierObject(bool createChildren)
{
}

EnergyBarrierObject::~EnergyBarrierObject()
{
	clear();
}

GameObject* EnergyBarrierObject::clone()
{
	EnergyBarrierObject* newObject = new EnergyBarrierObject(false);
	this->cloneParams(newObject);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);

	return newObject;
}

void EnergyBarrierObject::cloneParams(GameObject* obj)
{
	GameObject::cloneParams(obj);

	EnergyBarrierObject* barrier = (EnergyBarrierObject*)obj;
}

void EnergyBarrierObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void EnergyBarrierObject::update(float delta)
{
	GameObject::update(delta);

	for (int i = 0; i < 2; ++i)
	{
		if (generators[i])
		{
			generators[i]->Position = glm::vec2(this->Position.x + i * this->Size.x, this->Position.y);
			if (i == 0)
				generators[i]->Position.x -= generators[i]->Size.x;
			generators[i]->update(delta);
		}
	}

	for (int i = 0; i < shockersTime.size(); ++i)
	{
		shockersTime[i] -= delta;
		if (shockersTime[i] <= 0.0f)
		{
			attachedShockers.erase(attachedShockers.begin() + i);
			shockersTime.erase(shockersTime.begin() + i);
			objectsWithShockers.erase(objectsWithShockers.begin() + i);
			i--;
		}
	}

	// when this object appears, play sound effect. When it goes away from screen, stop it.
	if (!onceAppears && !isOffTheScreen(pLevel->getRenderer()->getCurrentScreenDimensions()))
	{
		onceAppears = true;
		if(!generatorSound)
			generatorSound = pLevel->playSound("GeneratorEffect", true);
	}else
		if (onceAppears && isOffTheScreen(pLevel->getRenderer()->getCurrentScreenDimensions()))
		{
			onceAppears = false;
			if (generatorSound)
			{
				generatorSound->stop();
				generatorSound->drop();
				generatorSound = NULL;
			}
		}
}

void EnergyBarrierObject::draw(bool useInstanced, int amount)
{
	GameObject::draw(useInstanced, amount);

	for (int i = 0; i < 2; ++i)
		if (generators[i])
			generators[i]->draw(useInstanced, amount);
}

void EnergyBarrierObject::resize()
{
	GameObject::resize();

	glm::vec2 screenRatio = pLevel->getRenderer()->getScreenRatio();
	float barrierRatio = Size.x / Size.y;

	Size.x = Size.x * screenRatio.x;
	Size.y = Size.x / barrierRatio;
	for (int i = 0; i < 2; ++i)
		if (generators[i])
		{
			float genRatio = generators[i]->Size.x / generators[i]->Size.y;
			generators[i]->Size.x = generators[i]->Size.x * screenRatio.x;
			generators[i]->Size.y = generators[i]->Size.x / genRatio;
			generators[i]->Position = glm::vec2(this->Position.x + i * this->Size.x, this->Position.y);
			if (i == 0)
				generators[i]->Position.x -= generators[i]->Size.x;
		}

	if (blastWave)
	{
		float waveRatio = blastWave->Size.x / blastWave->Size.y;
		blastWave->Size.x = blastWave->Size.x * screenRatio.x;
		blastWave->Size.y = blastWave->Size.x / waveRatio;
	}
}

void EnergyBarrierObject::makeCollision(GameObject* obj)
{
	if (this->readyForDeath || obj->getReadyForDeath() ||
		!pLevel->getBehaviour()->checkForCollisionAddiction(this, obj))
		return;

	for (int i = 0; i < 2; ++i)
		if (generators[i] == obj)
			return;

	if (visible)
	{
		obj->setHealth(obj->getHealth() - this->damage);
		attachElectricShockToObject(obj);
	}
}

void EnergyBarrierObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	GameObject::makeReaction(difference, otherObj, collisionChecker);
}

bool EnergyBarrierObject::notify(GameObject* notifiedObject, NotifyCode code)
{
	int generatorIndex = -1;
	for (int i = 0; i < 2; ++i)
		if (generators[i] == notifiedObject)
		{
			generatorIndex = i;
			break;
		}

	switch (code)
	{
	case NotifyCode::Destroyed:
	{
		if (generatorIndex < 0)
			return false;

		// if some of generators was destroyed, destroy second. 
		pLevel->removeObject(notifiedObject);
		generators[generatorIndex] = NULL;
		delete notifiedObject;

		if (generators[0] || generators[1])
		{
			setVisible(false);

			if (generators[0])
				spawnBlastWave(0);
			else
				spawnBlastWave(1);
		}
		else
			readyForDeath = true;

		return true;
	}
		break;
	case NotifyCode::BlastFinished:
		// sends when explosion blast wave was finished its animation.
		// so this is the time for destroying this object.
		readyForDeath = true;
		break;
	default:
		break;
	}

	return false;
}

void EnergyBarrierObject::hideFromLevel(bool hide)
{
	GameObject::hideFromLevel(hide);
	for (int i = 0; i < 2; ++i)
		if (generators[i])
			generators[i]->hideFromLevel(hide);
}

void EnergyBarrierObject::setGenerators(GameObject* left, GameObject* right)
{
	generators[0] = left;
	generators[1] = right;

	for (int i = 0; i < 2; ++i)
		if(generators[i])
			generators[i]->setParentObject(this);
}

void EnergyBarrierObject::setElectricShock(GameObject* obj)
{
	electricShock = obj;
	electricShock->setParentObject(this);
	electricShock->hideFromLevel(true);
	electricShock->setDamageAsAttachment(true);
}

void EnergyBarrierObject::setBlastWave(BlastWaveObject* wave)
{
	blastWave = wave;
	blastWave->hideFromLevel(true);
	blastWave->setParentObject(this);
}

void EnergyBarrierObject::setGeneratorSoundName(std::string name)
{
	generatorSoundName = name;
}

void EnergyBarrierObject::attachElectricShockToObject(GameObject* obj)
{
	// if this object has already got shocker, return.
	auto it = find(objectsWithShockers.begin(), objectsWithShockers.end(), obj);
	if (it != objectsWithShockers.end())
		return;

	GameObject* shock = electricShock->clone();
	shock->setLevel(this->pLevel);
	shock->startSelfDestroying(true);

	obj->attachNewObject(shock);
	attachedShockers.push_back(shock);
	shockersTime.push_back(1.5f);
	objectsWithShockers.push_back(obj);
}

void EnergyBarrierObject::spawnBlastWave(int generatorIndex)
{
	generators[generatorIndex]->setVisible(false);
	blastWave->setParentObject(NULL);
	blastWave->startSelfDestroying(true);
	blastWave->setSender(this);
	blastWave->hideFromLevel(false);
	blastWave->Position = generators[generatorIndex]->Position +
		glm::vec2(generators[generatorIndex]->Size.x / 2, generators[generatorIndex]->Size.y / 2) -
		glm::vec2(blastWave->Size.x / 2, blastWave->Size.y / 2);
}

void EnergyBarrierObject::clear()
{
	if (generatorSound)
	{
		generatorSound->stop();
		generatorSound = NULL;
	}

	// disconnect generators from this object and let them to self-destruct manually through level.
	for(int i = 0; i < 2; ++i)
		if (generators[i])
		{
			generators[i]->setParentObject(NULL);
			generators[i]->setReadyForDeath(true);
			generators[i] = NULL;
		}

	if (blastWave)
	{
		if (blastWave->getParentObject())
		{
			pLevel->removeObject(blastWave);
			delete blastWave;
		}
		else
		{
			blastWave->setParentObject(NULL);
			blastWave->setSender(NULL);
		}
		blastWave = NULL;
	}

	if (electricShock)
	{
		delete electricShock;
		electricShock = NULL;
	}
}