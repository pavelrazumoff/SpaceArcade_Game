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

void LevelBehaviour::setFinishLevelCallback(void(*actionCallback)(void))
{
	finishLevelCallback = actionCallback;
}

void LevelBehaviour::setIterateLevelCallback(void(*actionCallback)(void))
{
	updateLevelIterationCallback = actionCallback;
}

void LevelBehaviour::setTeleportPlayerCallback(void(*actionCallback)(GameObject*, LevelBehaviour*))
{
	teleportPlayerCallback = actionCallback;
}

void LevelBehaviour::clear()
{
	
}