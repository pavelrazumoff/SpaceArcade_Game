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
}

void BossShipAIController::setPawn(GameObject* pawn)
{
	BasicShipAIController::setPawn(pawn);
	controlledBossSpacecraft = dynamic_cast<BossSpacecraftObject*>(pawn);
}

void BossShipAIController::clear()
{

}