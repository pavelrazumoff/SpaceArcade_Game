/*
	* Space Arcade Game
	* Copyright (C) 2018 Paul Razumov.
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	* Source:
	*  https://github.com/pavelrazumoff/SpaceArcade_Game
*/

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