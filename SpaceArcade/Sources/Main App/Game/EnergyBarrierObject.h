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

	EnergyBarrierObject.h
	This class represents energy barrier object that has two generators on the sides of it
	and lightning charge in the center with animation. To destroy this type of object
	player has to defeat only one generator, so another will explode.
	Also, when player is passing through the lightning charge it gets electric shock,
	that will decrease its health points for a specific time.
*/

#include "GameObject.h"
#include "BlastWaveObject.h"

class EnergyBarrierObject : public GameObject
{
public:
	EnergyBarrierObject();
	EnergyBarrierObject(bool createChildren);
	virtual ~EnergyBarrierObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual void makeCollision(GameObject* obj);
	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	virtual bool notify(GameObject* notifiedObject, NotifyCode code);

	virtual void hideFromLevel(bool hide);
	void setGenerators(GameObject* left, GameObject* right);
	void setElectricShock(GameObject* obj);
	void setBlastWave(BlastWaveObject* wave);
	void setGeneratorSoundName(std::string name);

	void attachElectricShockToObject(GameObject* obj);
	void spawnBlastWave(int generatorIndex);

protected:
	GameObject* generators[2];		// 0 - left, 1 - right.
	GameObject* electricShock = NULL;
	BlastWaveObject* blastWave = NULL;

	std::string generatorSoundName;
	ISound* generatorSound = NULL;

	std::vector<GameObject*> attachedShockers;
	std::vector<float> shockersTime;
	// holds all objects, that got shockers. 
	// this needs to control, what object already got this and those, who didn't.
	std::vector<GameObject*> objectsWithShockers;

	bool onceAppears = false;		// false - was off the screen, true - on screen.
};