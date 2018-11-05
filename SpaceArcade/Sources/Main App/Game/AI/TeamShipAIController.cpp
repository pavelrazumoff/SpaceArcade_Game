#include "TeamShipAIController.h"

TeamShipAIController::TeamShipAIController()
{

}

TeamShipAIController::~TeamShipAIController()
{
	clear();
}

AIController* TeamShipAIController::clone()
{
	TeamShipAIController* newAI = new TeamShipAIController();
	newAI->init();
	this->cloneParams(newAI);

	return newAI;
}

void TeamShipAIController::cloneParams(AIController* ai)
{
	AIController::cloneParams(ai);
	TeamShipAIController* shipAI = (TeamShipAIController*)ai;
}

void TeamShipAIController::init()
{

}

void TeamShipAIController::update(float delta)
{
	if (!isActive || controllers.size() == 0)
		return;

	// if some of the controller's pawn is destroyed or something else, erase it from controllers list.
	for(int i = 0; i < controllers.size(); ++i)
		if ((controllers[i]->getPawn() && controllers[i]->getPawn()->getHealth() <= 0.0f) ||
			!controllers[i]->getPawn())
		{
			controllers.erase(controllers.begin() + i);
			--i;
		}

	followTargetEnemy(delta);
}

void TeamShipAIController::followTargetEnemy(float delta)
{
	// here we have to manage every teammate's ai controllers.
	if (!targetEnemy)
		return;

	// if target was destroyed, reset target pointer.
	if (targetEnemy->getHealth() <= 0.0f)
	{
		targetEnemy = NULL;
		return;
	}

	// first, divide screen on the monitoring areas.
	// every single ai controller will be monitoring specific area plot.
	glm::vec2 screenDimensions = targetEnemy->getLevel()->getRenderer()->getCurrentScreenDimensions();
	int areaWidth = screenDimensions.x / controllers.size();
	float areaStep = areaWidth / screenDimensions.x;

	// check, which ai controller is the nearest for the target enemy.
	int areaIndex = targetEnemy->Position.x / areaWidth;

	// if target is beyond screen.
	if (areaIndex < 0 || areaIndex >= controllers.size())
		return;

	// make this controller chase target.
	controllers[areaIndex]->setTargetEnemy(targetEnemy);

	// free other controllers from chasing target.
	for (int i = 0; i < controllers.size(); ++i)
	{
		if (i != areaIndex)
			controllers[i]->setTargetEnemy(NULL);
		controllers[i]->setControlledArea(glm::vec4(areaStep * i, 0.0f, areaStep * (i + 1), 1.0f));
	}
}

void TeamShipAIController::addController(AIController* controller)
{
	controllers.push_back(controller);
}

void TeamShipAIController::clear()
{

}