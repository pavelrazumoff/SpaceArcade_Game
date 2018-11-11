#pragma once
#include "BasicShipAIController.h"

class BossShipAIController : public BasicShipAIController
{
public:
	BossShipAIController();
	virtual ~BossShipAIController();

	virtual void init();
	virtual void update(float delta);

	virtual AIController* clone();
	virtual void cloneParams(AIController* ai);

	virtual void clear();

	virtual void setPawn(GameObject* pawn);

protected:
	BossSpacecraftObject* controlledBossSpacecraft = NULL;
};