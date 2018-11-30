#include "BossShipAIController.h"

BossShipAIController::BossShipAIController()
{

}

BossShipAIController::~BossShipAIController()
{
	clear();
}

AIController* BossShipAIController::clone()
{
	BossShipAIController* newAI = new BossShipAIController();
	newAI->init();
	this->cloneParams(newAI);

	return newAI;
}

void BossShipAIController::cloneParams(AIController* ai)
{
	BasicShipAIController::cloneParams(ai);
	BossShipAIController* shipAI = (BossShipAIController*)ai;
}

void BossShipAIController::init()
{

}

void BossShipAIController::update(float delta)
{
	if (!isActive || (controlledPawn && controlledPawn->getHealth() <= 0.0f))
		return;

	followTargetEnemy(delta);

	// if target is null, this ship has to return in its source position.
	if (!targetEnemy)
		returnToSourcePosition(delta);

	// if target is in the fire area.
	if (readyToFire)
	{
		timeToFire -= delta;
		// if it is time to fire, dew it.
		if (timeToFire <= 0.0f)
		{
			if(controlledBossSpacecraft)
				controlledBossSpacecraft->spawnLaserRays();
			// drop fire timing.
			timeToFire = fireFrequency;
		}
	}

	// recover health if this option is enabled.
	if (healthRecovery && disableHealthRecoveryTimer <= 0.0f)
	{
		if(controlledPawn->getHealth() < controlledPawn->getMaxHealth())
			controlledPawn->setHealth(controlledPawn->getHealth() + recoveredHealth * delta);
	}

	// if health recovery was disabled for a specific time, decrese this time.
	if(disableHealthRecoveryTimer > 0.0f)
		disableHealthRecoveryTimer -= delta;
}

void BossShipAIController::setPawn(GameObject* pawn)
{
	BasicShipAIController::setPawn(pawn);
	controlledBossSpacecraft = dynamic_cast<BossSpacecraftObject*>(pawn);
}

void BossShipAIController::enableHealthRecovery()
{
	healthRecovery = true;
}

void BossShipAIController::disableHealthRecovery()
{
	healthRecovery = false;
}

void BossShipAIController::setDisableHealthRecoveryTimer(float time)
{
	disableHealthRecoveryTimer = time;
}

void BossShipAIController::setHealthRecoveryValue(float health)
{
	recoveredHealth = health;
}

void BossShipAIController::clear()
{

}