#pragma once
#include "GameObject.h"

class SpacecraftObject : public GameObject
{
public:
	SpacecraftObject();
	SpacecraftObject(bool createChildren);
	virtual ~SpacecraftObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action, bool* key_pressed);

	virtual bool notify(GameObject* notifiedObject, NotifyCode code);

	void setLaserRay(GameObject* laser);

	GameObject* getLaserRay();
	GameObject* getRocket();

	void setEnergyChangedCallback(void(*actionCallback)(float, float));
	void setRocketIntegrityChangedCallback(void(*actionCallback)(int, int));

	void setControlVelocityByRotation(bool control);
	void setMaxEnergy(float energy);
	void setUsedEnergy(float energy);
	void setRocketDetail(int detail);
	void setRocketStartVelocity(glm::vec2 vel);
	void setRocketRelativePosition(glm::vec2 pos, int rocket_index);
	void setLaserSoundName(std::string name);
	void setRocketSoundName(std::string name);

	float getMaxEnergy();
	float getUsedEnergy();
	int getRocketIntegrity();
	int getRocketFreeIndex();

	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);
	void spawnLaserRay();
	void spawnRocket();

protected:
	GameObject* laser_ray;
	GameObject* pRocket;
	std::vector<GameObject*> laser_rays;
	std::vector<GameObject*> rockets;
	std::string laserSoundName;
	std::string rocketSoundName;

	float maxEnergy = 100.0f;
	float usedEnergy = 0.0f;
	bool exceedMaxEnergy = false;
	void(*energyChanged)(float, float) = NULL;

	int rocketIntegrity = 0;					// 100 - one rocket is construct and can be fired. 3x rockets is max.
	glm::vec2 rocketStartVelocity;
	glm::vec2 rocketRelativePoses[3];
	void(*rocketIntegrityChanged)(int, int) = NULL;
};