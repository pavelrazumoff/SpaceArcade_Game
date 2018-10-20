#include "GameLevel.h"

GameLevel::~GameLevel()
{
	clear();
}

void GameLevel::init(Texture2D* cubemap, Shader cubemapShader, SpriteRenderer* renderer)
{
	this->backgroundCubemap = cubemap;
	this->cubemapShader = cubemapShader;
	this->renderer = renderer;
}

void GameLevel::update(float delta)
{
	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getParentObject())
			objects[i]->update(delta);

	doCollisions();
}

void GameLevel::Draw()
{
	// draw skybox as last.
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content.

	cubemapShader.use();

	glm::mat4 modelMat = glm::mat4();
	modelMat = glm::rotate(modelMat, -(float)glfwGetTime() / 10, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::rotate(modelMat, -(float)glfwGetTime() / 100, glm::vec3(0.0f, 1.0f, 0.0f));

	cubemapShader.setMat4("model", modelMat);
	glActiveTexture(GL_TEXTURE0);
	backgroundCubemap->BindCubemap();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.

	glDisable(GL_DEPTH_TEST);

	// draw only those objects, whose parents are NULL.
	// other objects will be drawn inside those draw calls (such as laser rays of spacecraft).
	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getParentObject())
			objects[i]->Draw();

	glEnable(GL_DEPTH_TEST);
}

void GameLevel::resize()
{
	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getParentObject())
			objects[i]->resize();
}

void GameLevel::handleInput(GLFWwindow *window, float delta)
{
	for (int i = 0; i < objects.size(); ++i)
		objects[i]->handleInput(window, delta);
}

void GameLevel::processKey(int key, int action, bool* key_pressed)
{
	for (int i = 0; i < objects.size(); ++i)
		objects[i]->processKey(key, action, key_pressed);
}

void GameLevel::doCollisions()
{
	for (int i = 0; i < objects.size(); ++i)
	{
		for (int j = i + 1; j < objects.size(); ++j)
		{
			// check if two objects can do damage (exclude cases when damaging object collides with stars for example)
			// and also check collision for two objects.
			if (objects[i]->isDamagingObject() && objects[j]->isDamagingObject() &&
				objects[i]->getHealth() > 0.0f && objects[j]->getHealth() > 0.0f)
			{
				glm::vec2 diff;
				if (objects[i]->checkCollision(objects[j], diff))
				{
					objects[i]->makeCollision(objects[j]);
					objects[i]->makeReaction(diff, objects[j], true);
					objects[j]->makeReaction(diff, objects[i], false);
				}
			}
		}
	}

	for (int i = 0; i < objects.size(); ++i)
		if (objects[i]->getReadyForDeath())
		{
			GameObject* parentObj = objects[i]->getParentObject();
			if (parentObj)
				parentObj->notify(objects[i], NotifyCode::Destroyed);
			else
			{
				GameObject* obj = objects[i];
				removeObject(obj);
				delete obj;
			}

			i--;
		}
}

void GameLevel::addNewObject(GameObject* obj)
{
	objects.push_back(obj);
}

void GameLevel::removeObject(GameObject* obj)
{
	std::vector<GameObject*>::iterator it;

	it = find(objects.begin(), objects.end(), obj);
	if (it != objects.end())
		objects.erase(it);
}

void GameLevel::setScreenIndents(glm::vec4 indents)
{
	screenIndents = indents;
}

glm::vec4 GameLevel::getScreenIndents()
{
	return screenIndents;
}

SpriteRenderer* GameLevel::getRenderer()
{
	return renderer;
}

GLboolean GameLevel::IsCompleted()
{
	return GL_FALSE;
}

void GameLevel::clear()
{
	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getParentObject())
			delete objects[i];
	objects.clear();
}