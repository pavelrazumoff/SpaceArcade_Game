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
		laser_ray = new GameObject();
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
}

void BossSpacecraftObject::draw(bool useInstanced, int amount)
{
	SpacecraftObject::draw(useInstanced, amount);
}

void BossSpacecraftObject::resize()
{
	SpacecraftObject::resize();
}

void BossSpacecraftObject::notify(GameObject* notifiedObject, NotifyCode code)
{
	SpacecraftObject::notify(notifiedObject, code);
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
	
}