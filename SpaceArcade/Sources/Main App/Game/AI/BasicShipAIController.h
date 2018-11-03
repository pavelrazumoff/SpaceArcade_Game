#pragma once
/*
	BasicShipAIController.h
	This class helps to control basic enemy ship with laser firing and
	chasing on target.
*/

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

	// moves this ship after target over time.
	void followTargetEnemy(float delta);

	void setFireFrequency(float freq);
	virtual void setPawn(GameObject* pawn);

	float getFireFrequency();

protected:
	SpacecraftObject* controlledSpacecraft = NULL;

	// if true - enemy target is in the fire area and this ship can fire on it.
	bool readyToFire = false;
	// stores the remaining time, after which it spawn the laser and drops to the fireFrequency again.
	float timeToFire;
	// the frequency of laser firing.
	float fireFrequency = 0.3f;
};