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

	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

protected:
	GameObject* laser_ray;
	std::vector<GameObject*> laser_rays;
};