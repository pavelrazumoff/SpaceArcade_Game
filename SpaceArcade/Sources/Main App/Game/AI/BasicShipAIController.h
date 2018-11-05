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
	void returnToSourcePosition(float delta);

	void setFireFrequency(float freq);
	virtual void setPawn(GameObject* pawn);
	virtual void setSourcePosition(glm::vec2 pos);

	float getFireFrequency();
	glm::vec2 getSourcePosition();

protected:
	SpacecraftObject* controlledSpacecraft = NULL;

	// this is the position, to which pawn will go, if target enemy isn't set.
	// when using team control, this position is defined in controller's area bounds.
	// the position is set in (0, 1) bounds.
	glm::vec2 SourcePosition;

	// if true - enemy target is in the fire area and this ship can fire on it.
	bool readyToFire = false;
	// stores the remaining time, after which it spawn the laser and drops to the fireFrequency again.
	float timeToFire;
	// the frequency of laser firing.
	float fireFrequency = 0.3f;
};