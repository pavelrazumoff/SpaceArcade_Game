#pragma once
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

	AIController.h
	This class manages controlled pawn behaviour on the scene.
	It helps to simulate basic user input and often uses as enemy controller.
	AIController is mostly interface for derived ai classes from it.
*/

#include "../GameObject.h"
#include "../GameLevel.h"

class AIController
{
public:
	AIController();
	virtual ~AIController();

	virtual void init();
	virtual void update(float delta);

	virtual AIController* clone();
	virtual void cloneParams(AIController* ai);

	virtual void clear();

	// blocks ai behaviour until it'll be unblocked.
	void BlockAI();
	void unblockAI();

	// sets controlled pawn object.
	virtual void setPawn(GameObject* pawn);
	// sets target enemy object that derived ai class can use to targeting on.
	void setTargetEnemy(GameObject* target);
	void setControlledArea(glm::vec4 area);

	GameObject* getPawn();
	GameObject* getTargetEnemy();
	glm::vec4 getControlledArea();

protected:
	GameObject* controlledPawn = NULL;
	GameObject* targetEnemy = NULL;

	// defines the normalized area, in that pawn can be navigated.
	glm::vec4 controlledArea;

	// active on default.
	bool isActive = true;
};