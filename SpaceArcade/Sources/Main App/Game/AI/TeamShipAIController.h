#pragma once
/*
	TeamShipAIController.h
*/

#include "BasicShipAIController.h"

class TeamShipAIController : public AIController
{
public:
	TeamShipAIController();
	virtual ~TeamShipAIController();

	virtual void init();
	virtual void update(float delta);

	virtual AIController* clone();
	virtual void cloneParams(AIController* ai);

	virtual void clear();

	void followTargetEnemy(float delta);

	void addController(AIController* controller);

protected:
	std::vector<AIController*> controllers;
};