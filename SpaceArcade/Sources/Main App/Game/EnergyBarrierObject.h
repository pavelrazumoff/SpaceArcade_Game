#pragma once
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

	virtual void notify(GameObject* notifiedObject, NotifyCode code);

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
	std::vector<GameObject*> objectsWithShockers;

	bool onceAppears = false;		// false - was off the screen, true - on screen.
};