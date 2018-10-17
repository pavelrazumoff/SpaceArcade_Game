#pragma once
#include "SpacecraftObject.h"

class GameLevel
{
public:
	// Constructor
	GameLevel() { }
	~GameLevel();

	void						init(Texture2D* cubemap, Shader cubemapShader, SpriteRenderer* renderer);
	void						resize();
	void						clear();

	// Render level
	void						Draw();
	void						update(float delta);

	void						handleInput(GLFWwindow *window, float delta);
	void						processKey(int key, int action, bool* key_pressed);

	void						doCollisions();

	void						addNewObject(GameObject* obj);
	void						removeObject(GameObject* obj);

	void						setScreenIndents(glm::vec4 indents);
	glm::vec4					getScreenIndents();

	SpriteRenderer*				getRenderer();

	// Check if the level is completed.
	GLboolean					IsCompleted();

private:
	std::vector<GameObject*>	objects;

	Texture2D*					backgroundCubemap;
	Shader						cubemapShader;

	SpriteRenderer*				renderer = NULL;
	glm::vec4					screenIndents;
};
