#pragma once
/*
	BlackHoleObject.h
	This class represents black hole simulation object, that can be used
	to destroy all another objects, that will fall into it, or to teleport
	player controlled craft into another level (the teleportation itself is
	implemented in that craft class).
*/

#include "GameObject.h"

// represents current black hole type.
enum BlackHoleType
{
	BasicHole,
	Teleportation,
	Destroyer
};

class BlackHoleObject : public GameObject
{
public:
	BlackHoleObject();
	BlackHoleObject(bool createChildren);
	virtual ~BlackHoleObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void setBlackHoleType(int type);
	int getBlackHoleType();

protected:
	int blackHoleType = BlackHoleType::BasicHole;
};