#include "LevelBehaviour.h"

LevelBehaviour::LevelBehaviour()
{
	
}

LevelBehaviour::LevelBehaviour(GameLevel* level, ResourceManager* resManager)
{
	pLevel = level;
	if (level)
		level->setBehaviour(this);
	pResourceManager = resManager;
}

LevelBehaviour::~LevelBehaviour()
{
	clear();
}

void LevelBehaviour::init()
{
}

void LevelBehaviour::startBehaviour()
{
	
}

void LevelBehaviour::pauseBehaviour()
{
	
}

void LevelBehaviour::resumeBehaviour()
{
	
}

void LevelBehaviour::finishBehaviour()
{
	
}

void LevelBehaviour::resetBehaviour()
{
	playerObject = NULL;
	complexControllers.clear();
	userInput = true;
	levelMode = -1;
	levelIteration = 0;
}

void LevelBehaviour::update(float delta)
{
	
}

void LevelBehaviour::setLevel(GameLevel* level)
{
	pLevel = level;
}

void LevelBehaviour::blockUserInput()
{
	userInput = false;
}

void LevelBehaviour::unblockUserInput()
{
	userInput = true;
}

GameLevel* LevelBehaviour::getLevel()
{
	return pLevel;
}

GameObject* LevelBehaviour::getPlayerObject()
{
	return playerObject;
}

bool LevelBehaviour::isUserInputBlocked()
{
	return !userInput;
}

void LevelBehaviour::addComplexAIController(AIController* controller)
{
	complexControllers.push_back(controller);
}

void LevelBehaviour::clear()
{
	
}