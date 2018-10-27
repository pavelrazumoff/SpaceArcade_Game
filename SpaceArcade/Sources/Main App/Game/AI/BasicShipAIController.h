#pragma once
#include "AIController.h"

class BasicShipAIController : public AIController
{
public:
	BasicShipAIController();
	virtual ~BasicShipAIController();

	virtual void init();
	virtual void update(float delta);

	virtual AIController* clone();
	virtual void cloneParams(AIController* ai);

	virtual void clear();

	void followTargetEnemy(float delta);

	void setFireFrequency(float freq);
	virtual void setPawn(GameObject* pawn);

	float getFireFrequency();

protected:
	SpacecraftObject* controlledSpacecraft = NULL;

	bool readyToFire = false;
	float timeToFire;
	float fireFrequency = 0.3f;
};