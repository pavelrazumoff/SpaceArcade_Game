#pragma once
/*
	AIController.h
	This class manages controlled pawn behaviour on the scene.
	It helps to simulate basic user input and often uses as enemy controller.
	AIController is mostly interface for derived ai classes from it.
*/

#include "../GameObject.h"
#include "../GameLevel.h"

class AIController
{
public:
	AIController();
	virtual ~AIController();

	virtual void init();
	virtual void update(float delta);

	virtual AIController* clone();
	virtual void cloneParams(AIController* ai);

	virtual void clear();

	// blocks ai behaviour until it'll be unblocked.
	void BlockAI();
	void unblockAI();

	// sets controlled pawn object.
	virtual void setPawn(GameObject* pawn);
	// sets target enemy object that derived ai class can use to targeting on.
	void setTargetEnemy(GameObject* target);
	void setControlledArea(glm::vec4 area);

	GameObject* getPawn();
	GameObject* getTargetEnemy();
	glm::vec4 getControlledArea();

protected:
	GameObject* controlledPawn = NULL;
	GameObject* targetEnemy = NULL;

	// defines the normalized area, in that pawn can be navigated.
	glm::vec4 controlledArea;

	// active on default.
	bool isActive = true;
};