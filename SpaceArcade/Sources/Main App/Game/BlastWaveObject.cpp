#include "BlastWaveObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

BlastWaveObject::BlastWaveObject()
{
	damage = 2.0f;
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

	if (selfDestroyTime <= 0.0f)
	{
		if (pSender)
		{
			pSender->notify(this, NotifyCode::BlastFinished);
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

void BlastWaveObject::clear()
{
}