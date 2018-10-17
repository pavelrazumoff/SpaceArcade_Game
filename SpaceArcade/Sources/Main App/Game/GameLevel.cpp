#include "GameLevel.h"

GameLevel::~GameLevel()
{
	clear();
}

void GameLevel::init(Texture2D cubemap, Shader cubemapShader, SpriteRenderer* renderer)
{
	this->backgroundCubemap = cubemap;
	this->cubemapShader = cubemapShader;
	this->renderer = renderer;

	spacecraft = new SpacecraftObject();

	for (int i = 0; i < numOfAsteroids; ++i)
	{
		asteroids.push_back(new GameObject());
		asteroids.back()->setHealth(20.0f);
		asteroids.back()->setDamage(30.0f);
	}
}

void GameLevel::update(float delta)
{
	if(spacecraft)
		spacecraft->update(delta);
	for (int i = 0; i < asteroids.size(); ++i)
		asteroids[i]->update(delta);

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
	backgroundCubemap.BindCubemap();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.

	glDisable(GL_DEPTH_TEST);

	if(spacecraft)
		spacecraft->Draw();
	for (int i = 0; i < asteroids.size(); ++i)
		asteroids[i]->Draw();

	glEnable(GL_DEPTH_TEST);
}

void GameLevel::resize()
{
	if (spacecraft)
		spacecraft->resize();
	for (int i = 0; i < asteroids.size(); ++i)
		asteroids[i]->resize();
}

void GameLevel::handleInput(GLFWwindow *window, float delta)
{
	if (spacecraft)
		spacecraft->handleInput(window, delta);
}

void GameLevel::processKey(int key, int action, bool* key_pressed)
{
	if (spacecraft)
		spacecraft->processKey(key, action, key_pressed);
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
				objects[i]->checkCollision(objects[j]))
			{
				objects[i]->makeCollision(objects[j]);
				objects[i]->makeReaction();
				objects[j]->makeReaction();
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

	// also remove from specific objects of current level.
	it = find(asteroids.begin(), asteroids.end(), obj);
	if (it != asteroids.end())
	{
		asteroids.erase(it);
		return;
	}

	if (obj == spacecraft)
		spacecraft = NULL;
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
	if (spacecraft)
	{
		delete spacecraft;
		spacecraft = NULL;
	}

	for (int i = 0; i < asteroids.size(); ++i)
		delete asteroids[i];
	asteroids.clear();
}