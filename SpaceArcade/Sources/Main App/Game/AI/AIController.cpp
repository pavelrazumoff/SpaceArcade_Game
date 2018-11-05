#include "AIController.h"

AIController::AIController()
{

}

AIController::~AIController()
{
	clear();
}

AIController* AIController::clone()
{
	AIController* newAI = new AIController();
	newAI->init();
	this->cloneParams(newAI);

	return newAI;
}

void AIController::cloneParams(AIController* ai)
{
	ai->setTargetEnemy(this->targetEnemy);
}

void AIController::init()
{
	
}

void AIController::update(float delta)
{
	
}

void AIController::BlockAI()
{
	isActive = false;
}

void AIController::unblockAI()
{
	isActive = true;
}

void AIController::setPawn(GameObject* pawn)
{
	controlledPawn = pawn;
}

void AIController::setTargetEnemy(GameObject* target)
{
	targetEnemy = target;
}

void AIController::setControlledArea(glm::vec4 area)
{
	controlledArea = area;
}

GameObject* AIController::getPawn()
{
	return controlledPawn;
}

GameObject* AIController::getTargetEnemy()
{
	return targetEnemy;
}

glm::vec4 AIController::getControlledArea()
{
	return controlledArea;
}

void AIController::clear()
{
	
}