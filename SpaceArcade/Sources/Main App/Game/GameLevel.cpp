#include "GameLevel.h"
#include "AI/LevelBehaviour.h"

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

void GameLevel::startLevel()
{
	if (behaviour)
		behaviour->startBehaviour();

	resize();
}

void GameLevel::update(float delta)
{
	if (behaviour)
		behaviour->update(delta);

	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getParentObject() && !objects[i]->isHiddenFromLevel())
			objects[i]->update(delta);

	doCollisions();
}

void GameLevel::draw()
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
		if (!objects[i]->getParentObject() && !objects[i]->isHiddenFromLevel())
			objects[i]->Draw();

	glEnable(GL_DEPTH_TEST);
}

void GameLevel::resize()
{
	glm::vec2 screenRatio = renderer->getScreenRatio();
	playerRestrictionHeight = playerRestrictionHeight * screenRatio.y;

	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getParentObject())
			objects[i]->resize();
}

void GameLevel::handleInput(GLFWwindow *window, float delta)
{
	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getAIController() && objects[i]->getHealth() > 0 && !objects[i]->isHiddenFromLevel())
			objects[i]->handleInput(window, delta);
}

void GameLevel::processKey(int key, int action, bool* key_pressed)
{
	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getAIController() && objects[i]->getHealth() > 0 && !objects[i]->isHiddenFromLevel())
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
				objects[i]->getHealth() > 0.0f && objects[j]->getHealth() > 0.0f && 
				!objects[i]->isHiddenFromLevel() && !objects[j]->isHiddenFromLevel())
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
		if (objects[i]->getReadyForDeath() && !objects[i]->isHiddenFromLevel())
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

void GameLevel::setBehaviour(LevelBehaviour* behaviour)
{
	this->behaviour = behaviour;
}

void GameLevel::setScreenIndents(glm::vec4 indents)
{
	screenIndents = indents;
}

void GameLevel::setPlayerRestrictionHeight(float height)
{
	playerRestrictionHeight = height;
}

LevelBehaviour* GameLevel::getBehaviour()
{
	return behaviour;
}

int GameLevel::getObjectsSize()
{
	return objects.size();
}

GameObject* GameLevel::getObjectByIndex(int index)
{
	if (index < 0 || index >= objects.size())
		return NULL;

	return objects[index];
}

int GameLevel::getIndexByObject(GameObject* obj)
{
	for (int i = 0; i < objects.size(); ++i)
		if (objects[i] == obj)
			return i;

	return -1;
}

int	GameLevel::getObjectsSizeByType(int obj_type)
{
	int size = 0;
	for (int i = 0; i < objects.size(); ++i)
		if (objects[i]->getObjectType() == obj_type)
			size++;

	return size;
}

GameObject* GameLevel::getObjectByTypeIndex(int obj_type, int index)
{
	for (int i = 0, typeIndex = 0; i < objects.size(); ++i)
		if (objects[i]->getObjectType() == obj_type)
		{
			if (typeIndex++ == index)
				return objects[i];
		}

	return NULL;
}

glm::vec4 GameLevel::getScreenIndents()
{
	return screenIndents;
}

float GameLevel::getPlayerRestrictionHeight()
{
	return playerRestrictionHeight;
}

SpriteRenderer* GameLevel::getRenderer()
{
	return renderer;
}

void GameLevel::clear()
{
	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getParentObject())
			delete objects[i];
	objects.clear();
}