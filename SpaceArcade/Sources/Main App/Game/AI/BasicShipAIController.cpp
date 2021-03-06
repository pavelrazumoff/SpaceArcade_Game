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

#include "BasicShipAIController.h"

BasicShipAIController::BasicShipAIController()
{

}

BasicShipAIController::~BasicShipAIController()
{
	clear();
}

AIController* BasicShipAIController::clone()
{
	BasicShipAIController* newAI = new BasicShipAIController();
	newAI->init();
	this->cloneParams(newAI);

	return newAI;
}

void BasicShipAIController::cloneParams(AIController* ai)
{
	AIController::cloneParams(ai);
	BasicShipAIController* shipAI = (BasicShipAIController*)ai;
	shipAI->setFireFrequency(this->fireFrequency);
}

void BasicShipAIController::init()
{

}

void BasicShipAIController::update(float delta)
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
			if(controlledSpacecraft)
				controlledSpacecraft->spawnLaserRay();
			// drop fire timing.
			timeToFire = fireFrequency;
		}
	}
}

void BasicShipAIController::followTargetEnemy(float delta)
{
	if (!targetEnemy || controlledPawn->getHealth() <= 0.0f)
	{
		readyToFire = false;
		return;
	}

	// if target was destroyed, reset target pointer by making this ship untargeted.
	if (targetEnemy->getHealth() <= 0.0f)
	{
		targetEnemy = NULL;
		readyToFire = false;
		return;
	}

	// get current level pointer.
	GameLevel* pLevel = controlledPawn->getLevel();

	// if target isn't in the fire area.
	if (abs(controlledPawn->Position.x + controlledPawn->Size.x / 2 - (targetEnemy->Position.x + targetEnemy->Size.x / 2)) > 20.0f)
	{
		glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
		glm::vec2 initialScreenRatio = dimensions / pLevel->getRenderer()->getInitialScreenDimensions();

		glm::vec2 shift = glm::vec2(delta * controlledPawn->VelocityScale.x * initialScreenRatio.x, delta * controlledPawn->VelocityScale.y * initialScreenRatio.y);

		// move this ship after target.
		if (controlledPawn->Position.x + controlledPawn->Size.x / 2 < (targetEnemy->Position.x + targetEnemy->Size.x / 2))
			controlledPawn->Position.x += shift.x;
		else
			controlledPawn->Position.x -= shift.x;

		// drop firing.
		readyToFire = false;
	}
	else
	{
		// if this ship just arrived in the fire area, let it fire momentally.
		if (!readyToFire)
			timeToFire = 0.0f;
		readyToFire = true;
	}

	// if difference between this ship and target is small, stop it.
	if (abs(controlledPawn->Position.x + controlledPawn->Size.x / 2 - (targetEnemy->Position.x + targetEnemy->Size.x / 2)) < 2.0f)
		controlledPawn->Velocity.x = 0.0f;
}

void BasicShipAIController::returnToSourcePosition(float delta)
{
	GameLevel* pLevel = controlledPawn->getLevel();
	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();

	glm::vec2 returnPosition = glm::vec2((SourcePosition.x * (controlledArea.z - controlledArea.x) + controlledArea.x) * screenDimensions.x,
		controlledPawn->Position.y);
	glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 initialScreenRatio = dimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	glm::vec2 shift = glm::vec2(delta * controlledPawn->VelocityScale.x * initialScreenRatio.x, delta * controlledPawn->VelocityScale.y * initialScreenRatio.y);
	float currentPos = controlledPawn->Position.x + controlledPawn->Size.x / 2;

	if (abs(currentPos - returnPosition.x) > 5.0f)
	{
		// move this ship to the source position.
		if (currentPos < returnPosition.x)
			controlledPawn->Position.x += shift.x;
		else
			controlledPawn->Position.x -= shift.x;
	}
}

void BasicShipAIController::setPawn(GameObject* pawn)
{
	controlledPawn = pawn;
	controlledSpacecraft = dynamic_cast<SpacecraftObject*>(pawn);
}

void BasicShipAIController::setFireFrequency(float freq)
{
	fireFrequency = freq;
	timeToFire = freq;
}

void BasicShipAIController::setSourcePosition(glm::vec2 pos)
{
	SourcePosition = pos;
}

float BasicShipAIController::getFireFrequency()
{
	return fireFrequency;
}

glm::vec2 BasicShipAIController::getSourcePosition()
{
	return SourcePosition;
}

void BasicShipAIController::clear()
{

}