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
	followTargetEnemy(delta);

	if (readyToFire)
	{
		timeToFire -= delta;
		if (timeToFire <= 0.0f)
		{
			controlledSpacecraft->spawnLaserRay();
			timeToFire = fireFrequency;
		}
	}
}

void BasicShipAIController::followTargetEnemy(float delta)
{
	if (!targetEnemy || controlledPawn->getHealth() <= 0.0f)
		return;

	if (targetEnemy->getHealth() <= 0.0f)
	{
		targetEnemy = NULL;
		readyToFire = false;
		return;
	}

	GameLevel* pLevel = controlledPawn->getLevel();

	if (abs(controlledPawn->Position.x - targetEnemy->Position.x) > 20.0f)
	{
		glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
		glm::vec2 initialScreenRatio = dimensions / pLevel->getRenderer()->getInitialScreenDimensions();

		glm::vec2 shift = glm::vec2(delta * controlledPawn->VelocityScale.x * initialScreenRatio.x, delta * controlledPawn->VelocityScale.y * initialScreenRatio.y);

		if (controlledPawn->Position.x < targetEnemy->Position.x)
			controlledPawn->Position.x += shift.x;
		else
			controlledPawn->Position.x -= shift.x;

		readyToFire = false;
	}
	else
	{
		if (!readyToFire)
			timeToFire = 0.0f;
		readyToFire = true;
	}

	if (abs(controlledPawn->Position.x - targetEnemy->Position.x) < 5.0f)
		controlledPawn->Velocity.x = 0.0f;
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

float BasicShipAIController::getFireFrequency()
{
	return fireFrequency;
}

void BasicShipAIController::clear()
{

}