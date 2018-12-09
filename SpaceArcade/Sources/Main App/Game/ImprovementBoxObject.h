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

	ImprovementBoxObject.h
	This class represents improvement kit, that player's craft can get by colliding with it.
*/

#include "SpacecraftObject.h"

// this struct defines, what kind of improvement box object will have.
struct ImprovementStruct
{
	void improve(SpacecraftObject* spacecraft);

	float health;
	float energy;
	int rocketDetail;
	bool blackHole;
	int coinValue;

	bool useHealth = false;
	bool useEnergy = false;
	bool useRocketDetail = false;
	bool useBlackHole = false;
	bool useCoinValue = false;
};

class ImprovementBoxObject : public GameObject
{
public:
	ImprovementBoxObject();
	ImprovementBoxObject(bool createChildren);
	virtual ~ImprovementBoxObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void setImprovement(ImprovementStruct improv);
	void setSmokeObject(GameObject* smoke);

	ImprovementStruct getImprovement();
	GameObject* getSmokeObject();

protected:
	ImprovementStruct improvement;
	// this spawns after player's craft collides with this box.
	GameObject* smokeObject = NULL;
};