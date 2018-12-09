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

	SpaceStationObject.h
	This class defines space station, that can be used to trade some stuff.
*/

#include "SpacecraftObject.h"

enum StationState
{
	SpaceCraftPull,
	SpaceCraftLanded,
	SpaceCraftFree,
	StationFree
};

class SpaceStationObject : public GameObject
{
public:
	SpaceStationObject();
	SpaceStationObject(bool createChildren);
	virtual ~SpaceStationObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void pullSpaceCraft(float delta);
	void freeSpaceCraft(float delta);

	void setSpaceCraft(SpacecraftObject* spacecraft);
	void setLandingPoint(glm::vec2 point);
	void setStationState(int state);

	int getStationState();

	void setShowDialogueCallback(void(*actionCallback)(void));

protected:
	SpacecraftObject* pSpaceCraft = NULL;
	glm::vec2 landingPoint;
	int stationState = StationState::SpaceCraftPull;

	void(*showDialogueCallback)(void) = NULL;
};