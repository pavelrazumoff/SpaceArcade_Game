#pragma once
/*
	BlastWaveObject.h
	This class represents simple explosion wave, that spawned after
	some object was destroyed or in other cases and self destroys after
	animation clip will complete.
*/

#include "GameObject.h"

class BlastWaveObject : public GameObject
{
public:
	BlastWaveObject();
	BlastWaveObject(bool createChildren);
	virtual ~BlastWaveObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	void setSender(GameObject* sender);
	virtual void startSelfDestroying(bool start);

protected:
	// pointer to the initiator of spawning this object.
	GameObject* pSender = NULL;
};