#pragma once
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

protected:
	GameObject* pSender = NULL;
};