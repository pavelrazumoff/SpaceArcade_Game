#include "SpaceStationObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

SpaceStationObject::SpaceStationObject()
{
	damage = 0.0f;
	usePhysics = false;
	objectType = ObjectTypes::SpaceStation;
}

SpaceStationObject::SpaceStationObject(bool createChildren)
{
}

SpaceStationObject::~SpaceStationObject()
{
	clear();
}

GameObject* SpaceStationObject::clone()
{
	SpaceStationObject* newObject = new SpaceStationObject(false);
	this->cloneParams(newObject);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);

	return newObject;
}

void SpaceStationObject::cloneParams(GameObject* obj)
{
	GameObject::cloneParams(obj);

	SpaceStationObject* station = (SpaceStationObject*)obj;
}

void SpaceStationObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void SpaceStationObject::update(float delta)
{
	GameObject::update(delta);

	if(stationState == StationState::SpaceCraftPull)
		pullSpaceCraft(delta);
	if (stationState == StationState::SpaceCraftFree)
		freeSpaceCraft(delta);

	if (pSpaceCraft)
		pSpaceCraft->update(delta);
}

void SpaceStationObject::draw(bool useInstanced, int amount)
{
	GameObject::draw(useInstanced, amount);

	if (pSpaceCraft)
		pSpaceCraft->draw();
}

void SpaceStationObject::resize()
{
	GameObject::resize();

	if (pSpaceCraft)
	{
		pSpaceCraft->resize();
		if (stationState == StationState::SpaceCraftLanded)
			pSpaceCraft->Position = this->Position + landingPoint;
	}
}

void SpaceStationObject::makeCollision(GameObject* obj)
{
}

void SpaceStationObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	GameObject::makeReaction(difference, otherObj, collisionChecker);
}

void SpaceStationObject::pullSpaceCraft(float delta)
{
	if (!pSpaceCraft)
		return;

	glm::vec2 point = this->Position + landingPoint;

	if (abs(pSpaceCraft->Position.x - point.x) > 5.0f ||
		abs(pSpaceCraft->Position.y - point.y) > 5.0f)
	{
		glm::vec2 diff = point - pSpaceCraft->Position;
		diff = glm::normalize(diff);

		pSpaceCraft->Velocity = diff * glm::vec2(this->getImpulseFactor());
	}
	else
	{
		pSpaceCraft->Velocity = glm::vec2(0.0f);
		stationState = StationState::SpaceCraftLanded;
		if (showDialogueCallback)
			showDialogueCallback();
	}
}

void SpaceStationObject::freeSpaceCraft(float delta)
{
	if (!pSpaceCraft)
		return;

	glm::vec2 point = this->Position + glm::vec2(1.33f * this->Size.x, landingPoint.y);

	if (abs(pSpaceCraft->Position.x - point.x) > 5.0f ||
		abs(pSpaceCraft->Position.y - point.y) > 5.0f)
	{
		glm::vec2 diff = point - pSpaceCraft->Position;
		diff = glm::normalize(diff);

		pSpaceCraft->Velocity = diff * glm::vec2(this->getImpulseFactor());
	}
	else
	{
		pSpaceCraft->Velocity = glm::vec2(0.0f);
		stationState = StationState::StationFree;
	}
}

void SpaceStationObject::setSpaceCraft(SpacecraftObject* spacecraft)
{
	pSpaceCraft = spacecraft;
	if(pSpaceCraft)
		pSpaceCraft->setParentObject(this);
}

void SpaceStationObject::setLandingPoint(glm::vec2 point)
{
	landingPoint = point;
}

void SpaceStationObject::setStationState(int state)
{
	stationState = state;
}

int SpaceStationObject::getStationState()
{
	return stationState;
}

void SpaceStationObject::setShowDialogueCallback(void(*actionCallback)(void))
{
	showDialogueCallback = actionCallback;
}

void SpaceStationObject::clear()
{
	if (pSpaceCraft)
	{
		pSpaceCraft->setParentObject(NULL);
		pSpaceCraft = NULL;
	}
}