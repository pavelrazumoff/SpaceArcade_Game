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

	BossSpacecraftObject.h
	This class represents improved spacecraft object class.
	It can spawn many lasers in one time. Plus it has opportunity to
	enable energy shield, that can be destroyed only by specific object types
	such as rockets or ion charges.
*/

#include "SpacecraftObject.h"

class BossSpacecraftObject : public SpacecraftObject
{
public:
	BossSpacecraftObject();
	BossSpacecraftObject(bool createChildren);
	virtual ~BossSpacecraftObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual bool notify(GameObject* notifiedObject, NotifyCode code);

	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void spawnLaserRays();
	void enableShield(bool enable);
	bool isShieldEnabled();

	void setEnergyShield(GameObject* shield);
	GameObject* getEnergyShield();

	void addLaserStartPoint(glm::vec2 pos);
	void setIndexOfPreferredLaserPoint(int index);

protected:
	GameObject* pEnergyShieldTemplate = NULL;
	GameObject* energyShield = NULL;

	// relative laser starting positions.
	std::vector<glm::vec2>	lasersStartPoints;
	// this indicates, what laser starting point to use when there is a few energy
	// to produce all laser rays in current time.
	int indexOfPrefferedLaserPoint = 0;
};