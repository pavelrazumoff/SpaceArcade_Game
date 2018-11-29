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

void LevelBehaviour::restartLevelMusic()
{
}

void LevelBehaviour::update(float delta)
{
	
}

void LevelBehaviour::setLevel(GameLevel* level)
{
	pLevel = level;
}

void LevelBehaviour::setPlayerObject(GameObject* obj)
{
	playerObject = obj;
}

void LevelBehaviour::blockUserInput()
{
	userInput = false;
}

void LevelBehaviour::blockUserAttack()
{
	userAttack = false;
}

void LevelBehaviour::unblockUserInput()
{
	userInput = true;
}

void LevelBehaviour::unblockUserAttack()
{
	userAttack = true;
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

bool LevelBehaviour::isUserAttackBlocked()
{
	return !userAttack;
}

int LevelBehaviour::getLevelIteration()
{
	return levelIteration;
}

void LevelBehaviour::addComplexAIController(AIController* controller)
{
	complexControllers.push_back(controller);
}

void LevelBehaviour::teleport(GameObject* object)
{
	
}

void LevelBehaviour::clear()
{
	
}