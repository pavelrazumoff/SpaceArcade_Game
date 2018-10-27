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

	virtual void Draw();
	virtual void update(float delta);

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action, bool* key_pressed);

	virtual void notify(GameObject* notifiedObject, NotifyCode code);

	void setLaserRay(GameObject* laser);

	GameObject* getLaserRay();

	void setEnergyChangedCallback(void(*actionCallback)(float, float));

	void setControlVelocityByRotation(bool control);
	void setMaxEnergy(float energy);
	void setUsedEnergy(float energy);

	float getMaxEnergy();
	float getUsedEnergy();

	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);
	void spawnLaserRay();

protected:
	GameObject* laser_ray;
	std::vector<GameObject*> laser_rays;

	float maxEnergy = 100.0f;
	float usedEnergy = 0.0f;
	bool exceedMaxEnergy = false;
	void(*energyChanged)(float, float) = NULL;
};