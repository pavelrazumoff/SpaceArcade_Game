#include "BossSpacecraftObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

BossSpacecraftObject::BossSpacecraftObject()
{
	damage = 10.0f;
	explosionTime = 1.5f;
	usePhysics = true;
	objectType = ObjectTypes::BossSpaceCraft;
}

BossSpacecraftObject::BossSpacecraftObject(bool createChildren)
{
	if (createChildren)
		pLaserRay = new GameObject();
}

BossSpacecraftObject::~BossSpacecraftObject()
{
	clear();
}

GameObject* BossSpacecraftObject::clone()
{
	BossSpacecraftObject* newObject = new BossSpacecraftObject(false);
	this->cloneParams(newObject);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);

	return newObject;
}

void BossSpacecraftObject::cloneParams(GameObject* obj)
{
	SpacecraftObject::cloneParams(obj);

	BossSpacecraftObject* spacecraft = (BossSpacecraftObject*)obj;
}

void BossSpacecraftObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity)
{
	SpacecraftObject::init(level, pos, size, sprite, velocity);
}

void BossSpacecraftObject::update(float delta)
{
	SpacecraftObject::update(delta);

	if (energyShield)
	{
		energyShield->Position = this->Position + energyShield->RelativePosition;
		energyShield->update(delta);
	}
}

void BossSpacecraftObject::draw(bool useInstanced, int amount)
{
	SpacecraftObject::draw(useInstanced, amount);

	if (energyShield && !energyShield->isHiddenFromLevel())
		energyShield->draw(useInstanced, amount);
}

void BossSpacecraftObject::resize()
{
	SpacecraftObject::resize();

	if (energyShield)
		energyShield->resize();
}

bool BossSpacecraftObject::notify(GameObject* notifiedObject, NotifyCode code)
{
	if (SpacecraftObject::notify(notifiedObject, code))
		return true;

	switch (code)
	{
	case Destroyed:
	{
		pLevel->removeObject(notifiedObject);
		if (notifiedObject == energyShield)
			energyShield = NULL;
		delete notifiedObject;

		return true;
	}
	break;
	default:
		break;
	}

	return false;
}

void BossSpacecraftObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	SpacecraftObject::makeReaction(difference, otherObj, collisionChecker);

	if (!otherObj->isUsePhysics())
		return;
}

void BossSpacecraftObject::spawnLaserRays()
{
	if (maxEnergy - usedEnergy > maxEnergy / 2)
	{
		for (int i = 0; i < lasersStartPoints.size(); ++i)
		{
			spawnLaserRay();
			laser_rays.back()->Position = glm::vec2(Position.x + lasersStartPoints[i].x, Position.y + lasersStartPoints[i].y);
		}
	}
	else
		if(lasersStartPoints.size() > 0 && indexOfPrefferedLaserPoint >= 0 &&
			indexOfPrefferedLaserPoint < lasersStartPoints.size())
		{
			spawnLaserRay();
			laser_rays.back()->Position = glm::vec2(Position.x + lasersStartPoints[indexOfPrefferedLaserPoint].x,
				Position.y + lasersStartPoints[indexOfPrefferedLaserPoint].y);
		}
}

void BossSpacecraftObject::enableShield(bool enable)
{
	if (pEnergyShieldTemplate)
	{
		if (enable)
		{
			if (!energyShield)
			{
				energyShield = pEnergyShieldTemplate->clone();
				energyShield->setParentObject(this);
			}
			else
				energyShield->hideFromLevel(false);
		}else
			if (!enable && energyShield)
				energyShield->hideFromLevel(true);
	}
}

bool BossSpacecraftObject::isShieldEnabled()
{
	if (energyShield)
		return energyShield->isHiddenFromLevel();
	return false;
}

void BossSpacecraftObject::setEnergyShield(GameObject* shield)
{
	pEnergyShieldTemplate = shield;
	if (pEnergyShieldTemplate)
	{
		pEnergyShieldTemplate->setParentObject(this);
		pEnergyShieldTemplate->hideFromLevel(true);
	}
}

GameObject* BossSpacecraftObject::getEnergyShield()
{
	return pEnergyShieldTemplate;
}

void BossSpacecraftObject::addLaserStartPoint(glm::vec2 pos)
{
	lasersStartPoints.push_back(pos);
}

void BossSpacecraftObject::setIndexOfPreferredLaserPoint(int index)
{
	indexOfPrefferedLaserPoint = index;
}

void BossSpacecraftObject::clear()
{
	if (energyShield)
	{
		//energyShield->setParentObject(NULL);
		pLevel->removeObject(energyShield);
		delete energyShield;
		energyShield = NULL;
	}

	if (pEnergyShieldTemplate)
	{
		pLevel->removeObject(pEnergyShieldTemplate);
		delete pEnergyShieldTemplate;
		pEnergyShieldTemplate = NULL;
	}
}