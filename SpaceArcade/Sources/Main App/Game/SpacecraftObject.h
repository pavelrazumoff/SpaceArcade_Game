#pragma once
#include "GameObject.h"

class SpacecraftObject : public GameObject
{
public:
	SpacecraftObject();
	~SpacecraftObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual void Draw();
	virtual void update(float delta);

	void handleInput(GLFWwindow *window, float delta);
	void processKey(int key, int action, bool* key_pressed);

	GameObject* getLaserRay();

protected:
	GameObject* laser_ray;

	std::vector<glm::vec2> lasers_poses;
};