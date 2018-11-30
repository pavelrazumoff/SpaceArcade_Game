#pragma once
/*
	BossShipAIController.h
	This class helps to control boss enemy ship with opportunity to
	recover health and multiple laser firings in one time.
*/

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

	void enableHealthRecovery();
	void disableHealthRecovery();

	// if this > 0 then controlled pawn won't regenerate health for this time.
	void setDisableHealthRecoveryTimer(float time);
	void setHealthRecoveryValue(float health);

protected:
	BossSpacecraftObject* controlledBossSpacecraft = NULL;

	// enabling health recovery option.
	bool healthRecovery = false;
	// if this > 0.0 then health recovery will not work.
	float disableHealthRecoveryTimer = 0.0f;
	// holds, how much health will be recovered in one second.
	float recoveredHealth = 1.0f;
};