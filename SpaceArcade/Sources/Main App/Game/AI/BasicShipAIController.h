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

	BasicShipAIController.h
	This class helps to control basic enemy ship with laser firing and
	chasing on target.
*/

#include "AIController.h"

class BasicShipAIController : public AIController
{
public:
	BasicShipAIController();
	virtual ~BasicShipAIController();

	virtual void init();
	virtual void update(float delta);

	virtual AIController* clone();
	virtual void cloneParams(AIController* ai);

	virtual void clear();

	// moves this ship after target over time.
	void followTargetEnemy(float delta);
	void returnToSourcePosition(float delta);

	void setFireFrequency(float freq);
	virtual void setPawn(GameObject* pawn);
	virtual void setSourcePosition(glm::vec2 pos);

	float getFireFrequency();
	glm::vec2 getSourcePosition();

protected:
	SpacecraftObject* controlledSpacecraft = NULL;

	// this is the position, to which pawn will go if target enemy isn't set.
	// when using team control, this position is defined in controller's area bounds.
	// the position is set in (0, 1) normalized bounds.
	glm::vec2 SourcePosition;

	// if true - enemy target is in the fire area and this ship can fire on it.
	bool readyToFire = false;
	// stores the remaining time, after which it spawn the laser and drops to the fireFrequency again.
	float timeToFire;
	// the frequency of laser firing.
	float fireFrequency = 0.3f;
};