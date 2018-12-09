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