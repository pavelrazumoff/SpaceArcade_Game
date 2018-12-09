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

#include "BlastWaveObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

BlastWaveObject::BlastWaveObject()
{
	damage = 2.0f;
	impulseFactor = 300;
	usePhysics = true;
	useAnimation = true;
	objectType = ObjectTypes::BlastWave;
}

BlastWaveObject::BlastWaveObject(bool createChildren)
{
}

BlastWaveObject::~BlastWaveObject()
{
	clear();
}

GameObject* BlastWaveObject::clone()
{
	BlastWaveObject* newObject = new BlastWaveObject(false);
	this->cloneParams(newObject);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);

	return newObject;
}

void BlastWaveObject::cloneParams(GameObject* obj)
{
	GameObject::cloneParams(obj);

	BlastWaveObject* wave = (BlastWaveObject*)obj;
}

void BlastWaveObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void BlastWaveObject::update(float delta)
{
	GameObject::update(delta);

	// if self destroying timer is complete, send appropriate message to the sender
	// and clear all connections with it.
	if (selfDestroyTime <= 0.0f)
	{
		if (pSender)
		{
			pSender->notify(this, NotifyCode::BlastFinished);
			setParentObject(NULL);
			pSender = NULL;
		}
	}
}

void BlastWaveObject::draw(bool useInstanced, int amount)
{
	GameObject::draw(useInstanced, amount);
}

void BlastWaveObject::resize()
{
	GameObject::resize();

	glm::vec2 screenRatio = pLevel->getRenderer()->getScreenRatio();

	Size = glm::vec2(Size.x * screenRatio.x, Size.y * screenRatio.y);
}

void BlastWaveObject::setSender(GameObject* sender)
{
	pSender = sender;
}

void BlastWaveObject::startSelfDestroying(bool start)
{
	selfDestroying = start;
	// plays explosion sound when self destroying was started.
	if (explosionSoundName.compare("") && !isOffTheScreen(pLevel->getRenderer()->getCurrentScreenDimensions()))
	{
		pLevel->playSound(explosionSoundName, false);
		explosionSoundName = "";
	}
}

void BlastWaveObject::clear()
{
}