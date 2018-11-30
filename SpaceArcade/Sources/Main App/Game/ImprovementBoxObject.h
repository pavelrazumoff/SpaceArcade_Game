#pragma once
/*
	ImprovementBoxObject.h
	This class represents improvement kit, that player's craft can get by colliding with it.
*/

#include "SpacecraftObject.h"

// this struct defines, what kind of improvement box object will have.
struct ImprovementStruct
{
	void improve(SpacecraftObject* spacecraft);

	float health;
	float energy;
	int rocketDetail;
	bool blackHole;
	int coinValue;

	bool useHealth = false;
	bool useEnergy = false;
	bool useRocketDetail = false;
	bool useBlackHole = false;
	bool useCoinValue = false;
};

class ImprovementBoxObject : public GameObject
{
public:
	ImprovementBoxObject();
	ImprovementBoxObject(bool createChildren);
	virtual ~ImprovementBoxObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void setImprovement(ImprovementStruct improv);
	void setSmokeObject(GameObject* smoke);

	ImprovementStruct getImprovement();
	GameObject* getSmokeObject();

protected:
	ImprovementStruct improvement;
	// this spawns after player's craft collides with this box.
	GameObject* smokeObject = NULL;
};