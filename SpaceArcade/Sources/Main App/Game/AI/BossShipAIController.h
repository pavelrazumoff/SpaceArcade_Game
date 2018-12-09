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

	BossShipAIController.h
	This class helps to control boss enemy ship with opportunity to
	recover health and multiple laser firings in one time.
*/

#include "BasicShipAIController.h"

class BossShipAIController : public BasicShipAIController
{
public:
	BossShipAIController();
	virtual ~BossShipAIController();

	virtual void init();
	virtual void update(float delta);

	virtual AIController* clone();
	virtual void cloneParams(AIController* ai);

	virtual void clear();

	virtual void setPawn(GameObject* pawn);

	void enableHealthRecovery();
	void disableHealthRecovery();

	// if this > 0 then controlled pawn won't regenerate health for this time.
	void setDisableHealthRecoveryTimer(float time);
	void setHealthRecoveryValue(float health);

protected:
	BossSpacecraftObject* controlledBossSpacecraft = NULL;

	// enabling health recovery option.
	bool healthRecovery = false;
	// if this > 0.0 then health recovery will not work.
	float disableHealthRecoveryTimer = 0.0f;
	// holds, how much health will be recovered in one second.
	float recoveredHealth = 1.0f;
};