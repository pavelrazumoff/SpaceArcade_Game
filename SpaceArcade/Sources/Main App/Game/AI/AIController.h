#pragma once
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

	void BlockAI();
	void unblockAI();

	virtual void setPawn(GameObject* pawn);
	void setTargetEnemy(GameObject* target);

	GameObject* getPawn();
	GameObject* getTargetEnemy();

protected:
	GameObject* controlledPawn = NULL;
	GameObject* targetEnemy = NULL;

	bool isActive = true;
};