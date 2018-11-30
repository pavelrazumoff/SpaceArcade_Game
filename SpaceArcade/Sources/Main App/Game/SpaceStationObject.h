#pragma once
/*
	SpaceStationObject.h
	This class defines space station, that can be used to trade some stuff.
*/

#include "SpacecraftObject.h"

enum StationState
{
	SpaceCraftPull,
	SpaceCraftLanded,
	SpaceCraftFree,
	StationFree
};

class SpaceStationObject : public GameObject
{
public:
	SpaceStationObject();
	SpaceStationObject(bool createChildren);
	virtual ~SpaceStationObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void pullSpaceCraft(float delta);
	void freeSpaceCraft(float delta);

	void setSpaceCraft(SpacecraftObject* spacecraft);
	void setLandingPoint(glm::vec2 point);
	void setStationState(int state);

	int getStationState();

	void setShowDialogueCallback(void(*actionCallback)(void));

protected:
	SpacecraftObject* pSpaceCraft = NULL;
	glm::vec2 landingPoint;
	int stationState = StationState::SpaceCraftPull;

	void(*showDialogueCallback)(void) = NULL;
};