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

	QTRectangle rect;
	rect.x = 0.0f;
	rect.y = 0.0f;
	rect.Width = renderer->getCurrentScreenDimensions().x;
	rect.Height = renderer->getCurrentScreenDimensions().y;

	quadTree = new QuadTree(0, rect);
}

void GameLevel::startLevel()
{
	levelStatus = LevelStatus::Playing;
	setUseInstancing(ObjectTypes::Meteorite, true);
	setUseInstancing(ObjectTypes::Coin, true);
	setUseInstancing(ObjectTypes::LaserRay, false);
	setUseInstancing(ObjectTypes::SpaceCraft, false);
	setUseInstancing(ObjectTypes::Basic, false);
	setUseInstancing(ObjectTypes::EnergyBarrier, false);

	if (behaviour)
		behaviour->startBehaviour();
}

void GameLevel::pauseLevel()
{
	levelStatus = LevelStatus::Paused;
	if(behaviour)
		behaviour->pauseBehaviour();
}

void GameLevel::resumeLevel()
{
	levelStatus = LevelStatus::Playing;
	if (behaviour)
		behaviour->resumeBehaviour();
}

void GameLevel::finishLevel()
{
	levelStatus = LevelStatus::Overed;
}

void GameLevel::resetLevel()
{
	levelStatus = LevelStatus::NotStarted;

	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getParentObject())
			delete objects[i];
	objects.clear();

	levelTime = 0.0f;
	score = 0;

	for (auto it = typeObjects.begin(); it != typeObjects.end(); ++it)
		if (useInstancing[it->first])
		{
			if (objectsMatrices[it->first])
			{
				delete objectsMatrices[it->first];
				objectsMatrices[it->first] = NULL;
			}
		}

	typeObjects.clear();
	instancedObjects.clear();
	normalObjects.clear();
	objectsMatrices.clear();
	useInstancing.clear();

	addScore(0);
	if (behaviour)
		behaviour->resetBehaviour();
}

void GameLevel::update(float delta)
{
	if (levelStatus == LevelStatus::Paused)
		return;

	levelTime += delta;

	if (behaviour && levelStatus != LevelStatus::Overed)
		behaviour->update(delta);

	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getParentObject() && !objects[i]->isHiddenFromLevel())
			objects[i]->update(delta);

	doCollisions();
	
	// here we sort all of our visible objects on the arrays of instances (one object look, that'll be draw many times),
	// same type of instanced but have different look (explosions) and basic non-instanced objects.
	updateInstances();
}

void GameLevel::updatePaused(float delta)
{
	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getParentObject() && !objects[i]->isHiddenFromLevel())
			objects[i]->updatePaused(delta);
}

void GameLevel::updateInstances()
{
	glm::vec2 screenDimensions = renderer->getCurrentScreenDimensions();

	// clear previous arrays.
	instancedObjects.clear();
	normalObjects.clear();

	// go through all of object types and corresponding objects on the whole level.
	for (auto it = typeObjects.begin(); it != typeObjects.end(); ++it)
	{
		// if this object type uses instancing.
		if (useInstancing[it->first])
		{
			// go through all objects of this type and check, if every object is appropriate for rendering.
			for (int i = 0; i < it->second.size(); ++i)
				if (!it->second[i]->getParentObject() && !it->second[i]->isHiddenFromLevel() && !it->second[i]->isOffTheScreen(screenDimensions))
				{
					// if this object is not exploding yet, put it to the instanced array,
					// else put to the normals.
					if (it->second[i]->getHealth() > 0.0f)
						instancedObjects[it->first].push_back(it->second[i]);
					else
						normalObjects[it->first].push_back(it->second[i]);
				}

			// get size of instanced objects of this type.
			int renderSize = instancedObjects[it->first].size();
			if (renderSize > 0)
			{
				// fill model matrices array to be able to render this type of object many times with one draw call.
				glm::mat4* modelMatrices = new glm::mat4[renderSize];

				for (int i = 0; i < renderSize; ++i)
					modelMatrices[i] = instancedObjects[it->first][i]->getModel();

				setInstancesTransforms(it->first, modelMatrices, renderSize);
			}
		}
	}
}

void GameLevel::draw()
{
	// draw skybox as last.
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content.

	cubemapShader.use();

	glm::mat4 modelMat = glm::mat4();
	modelMat = glm::rotate(modelMat, -(float)levelTime / 10, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::rotate(modelMat, -(float)levelTime / 100, glm::vec3(0.0f, 1.0f, 0.0f));

	cubemapShader.setMat4("model", modelMat);
	glActiveTexture(GL_TEXTURE0);
	backgroundCubemap->BindCubemap();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.

	glDisable(GL_DEPTH_TEST);

	glm::vec2 screenDimensions = renderer->getCurrentScreenDimensions();
	
	// first we have to draw all instanced objects (one draw call for every type).
	for (auto it = instancedObjects.begin(); it != instancedObjects.end(); ++it)
	{
		if (it->second.size() > 0)
		{
			GLuint buffer;
			configureInstancedArray(it->first, buffer);
			it->second[0]->draw(true, it->second.size());

			glDeleteBuffers(1, &buffer);
		}
	}

	// second draw all normal objects as usual.
	for (auto it = normalObjects.begin(); it != normalObjects.end(); ++it)
	{
		for (int i = 0; i < it->second.size(); ++i)
			it->second[i]->draw();
	}
	
	// at last draw non-instanced objects as usual.
	for (auto it = typeObjects.begin(); it != typeObjects.end(); ++it)
	{
		if (!useInstancing[it->first])
		{
			for (int i = 0; i < it->second.size(); ++i)
				if (!it->second[i]->getParentObject() && !it->second[i]->isHiddenFromLevel() && !it->second[i]->isOffTheScreen(screenDimensions))
					it->second[i]->draw();
		}
	}

	glEnable(GL_DEPTH_TEST);
}

void GameLevel::resize()
{
	glm::vec2 screenRatio = renderer->getScreenRatio();
	playerRestrictionHeight = playerRestrictionHeight * screenRatio.y;

	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getParentObject() && objects[i]->getLevel() == this)
			objects[i]->resize();

	for (int i = 0; i < objects.size(); ++i)
		if(objects[i]->getLevel() == this)
			objects[i]->updateModelMatrix();
}

void GameLevel::handleInput(GLFWwindow *window, float delta)
{
	if (levelStatus == LevelStatus::Paused)
		return;

	if (behaviour && behaviour->isUserInputBlocked())
		return;

	for (int i = 0; i < objects.size(); ++i)
		if(!objects[i]->getAIController() && objects[i]->getHealth() > 0 && !objects[i]->isHiddenFromLevel())
			objects[i]->handleInput(window, delta);
}

void GameLevel::processKey(int key, int action, bool* key_pressed)
{
	if (levelStatus == LevelStatus::Paused)
		return;

	if (behaviour && behaviour->isUserInputBlocked())
		return;

	if (behaviour && behaviour->isUserAttackBlocked())
		return;

	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getAIController() && objects[i]->getHealth() > 0 && !objects[i]->isHiddenFromLevel())
			objects[i]->processKey(key, action, key_pressed);
}

void GameLevel::doCollisions()
{
	glm::vec2 screenDimensions = renderer->getCurrentScreenDimensions();

	QTRectangle rect;
	rect.x = 0.0f;
	rect.y = 0.0f;
	rect.Width = screenDimensions.x;
	rect.Height = screenDimensions.y;

	quadTree->reset(rect);

	//Insert all the objects's rects in the QuadTree algorithm.
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->isDamagingObject() && objects[i]->getHealth() > 0.0f &&
			!objects[i]->isHiddenFromLevel() && !objects[i]->isOffTheScreen(screenDimensions) &&
			objects[i]->isCheckingCollision())
		{
			rect.x = objects[i]->Position.x;
			rect.y = objects[i]->Position.y;
			rect.Width = objects[i]->Size.x;
			rect.Height = objects[i]->Size.y;
			rect.object_index = i;

			quadTree->insert(rect);
		}
	}

	std::vector<QTRectangle> returnObjects;
	std::map<GameObject*, std::vector<GameObject*>> collidedList;

	for (int i = 0; i < objects.size(); ++i)
	{
		if (objects[i]->isDamagingObject() && objects[i]->getHealth() > 0.0f &&
			!objects[i]->isHiddenFromLevel() && !objects[i]->isOffTheScreen(screenDimensions) &&
			objects[i]->isCheckingCollision())
		{
			QTRectangle rect;
			rect.x = objects[i]->Position.x;
			rect.y = objects[i]->Position.y;
			rect.Width = objects[i]->Size.x;
			rect.Height = objects[i]->Size.y;
			rect.object_index = i;

			returnObjects.clear();
			quadTree->retrieve(returnObjects, rect);

			for (int j = 0; j < returnObjects.size(); ++j)
			{
				int currentIndex = returnObjects[j].object_index;

				if (objects[i] == objects[currentIndex])
					continue;

				if (!behaviour->checkForCollisionAddiction(objects[i], objects[currentIndex]) &&
					!behaviour->checkForCollisionAddiction(objects[currentIndex], objects[i]))
					continue;

				auto it = collidedList.find(objects[currentIndex]);
				if (it != collidedList.end())
				{
					auto it2 = find(it->second.begin(), it->second.end(), objects[i]);
					if (it2 != it->second.end())
						continue;
				}

				// check if two objects can do damage (exclude cases when damaging object collides with stars for example)
				// and also check collision for two objects.
				if (objects[currentIndex]->isDamagingObject() && objects[currentIndex]->getHealth() > 0.0f &&
					!objects[currentIndex]->isHiddenFromLevel() && !objects[currentIndex]->isOffTheScreen(screenDimensions) &&
					objects[currentIndex]->isCheckingCollision())
				{
					collidedList[objects[i]].push_back(objects[currentIndex]);

					glm::vec2 diff;
					if (objects[i]->checkCollision(objects[currentIndex], diff))
					{
						objects[i]->makeCollision(objects[currentIndex]);
						objects[currentIndex]->makeCollision(objects[i]);

						objects[i]->makeReaction(diff, objects[currentIndex], true);
						objects[currentIndex]->makeReaction(diff, objects[i], false);
					}
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
	typeObjects[obj->getObjectType()].push_back(obj);

	if(typeObjects[obj->getObjectType()].size() == 0)
		objectsMatrices[obj->getObjectType()] = NULL;
}

void GameLevel::addExternalObject(GameObject* obj)
{
	addNewObject(obj);
	externalObjects.push_back(obj);
}

void GameLevel::removeObject(GameObject* obj)
{
	std::vector<GameObject*>::iterator it;

	it = find(objects.begin(), objects.end(), obj);
	if (it != objects.end())
		objects.erase(it);

	auto it2 = typeObjects.find(obj->getObjectType());
	if (it2 != typeObjects.end())
	{
		auto it3 = find(it2->second.begin(), it2->second.end(), obj);
		if (it3 != it2->second.end())
			it2->second.erase(it3);
	}
}

void GameLevel::removeExternalObject(GameObject* obj)
{
	removeObject(obj);

	std::vector<GameObject*>::iterator it;
	it = find(externalObjects.begin(), externalObjects.end(), obj);
	if (it != externalObjects.end())
		externalObjects.erase(it);
}

void GameLevel::addSound(std::string soundName, std::string soundPath)
{
	soundNames[soundName] = soundPath;
}

ISound* GameLevel::playSound(std::string soundName, bool loop)
{
	auto it = soundNames.find(soundName);
	if (it != soundNames.end())
		return pSoundEngine->play2D(soundNames[soundName].c_str(), loop, false, true);
	return NULL;
}

void GameLevel::setScoreChangedCallback(void(*actionCallback)(int))
{
	scoreChanged = actionCallback;
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

void GameLevel::setUseInstancing(int object_type, bool use)
{
	useInstancing[object_type] = use;
}

void GameLevel::setInstancesTransforms(int object_type, glm::mat4* transforms, int size)
{
	// here we update renderer vertex array to support instancing.

	// clear previous matrices array.
	if (objectsMatrices[object_type])
	{
		delete objectsMatrices[object_type];
		objectsMatrices[object_type] = NULL;
	}

	objectsMatrices[object_type] = transforms;
}

void GameLevel::configureInstancedArray(int object_type, GLuint &buffer)
{
	// if there is no current type in types array or there is no objects of this type, return.
	auto it = typeObjects.find(object_type);
	if (it == typeObjects.end() || it->second.size() == 0)
		return;

	// configure instanced array
	// -------------------------
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, instancedObjects[object_type].size() * sizeof(glm::mat4), &objectsMatrices[object_type][0], GL_STATIC_DRAW);

	glBindVertexArray(renderer->getVAO());

	// set attribute pointers for matrix (4 times vec4)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GameLevel::setSoundEnginePointer(ISoundEngine* soundEngine)
{
	pSoundEngine = soundEngine;
}

void GameLevel::addScore(int score)
{
	this->score += score;

	if (scoreChanged)
		scoreChanged(this->score);
}

int GameLevel::getScore()
{
	return score;
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

std::vector<GameObject*> GameLevel::getObstaclesInArea(glm::vec4 area, GameObject* excludeObject)
{
	// It returns all objects, that is in the specified area (x, y, x + w, y + h).
	std::vector<GameObject*> obstacles;

	for (int i = 0; i < objects.size(); ++i)
	{
		if (objects[i] == excludeObject)
			continue;

		// Collision x-axis?
		bool collisionX = objects[i]->Position.x + objects[i]->Size.x >= area.x &&
			area.z >= objects[i]->Position.x;
		// Collision y-axis?
		bool collisionY = objects[i]->Position.y + objects[i]->Size.y >= area.y &&
			area.w >= objects[i]->Position.y;

		if (collisionX && collisionY)
			obstacles.push_back(objects[i]);
	}

	return obstacles;
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

int GameLevel::getLevelStatus()
{
	return levelStatus;
}

void GameLevel::clear()
{
	if (quadTree)
	{
		quadTree->clear();
		delete quadTree;
		quadTree = NULL;
	}

	for (auto it = typeObjects.begin(); it != typeObjects.end(); ++it)
		if(useInstancing[it->first])
		{
			if (objectsMatrices[it->first])
			{
				delete objectsMatrices[it->first];
				objectsMatrices[it->first] = NULL;
			}
		}

	// remove all external objects from base objects array.
	for (int i = 0; i < externalObjects.size(); ++i)
	{
		auto it = std::find(objects.begin(), objects.end(), externalObjects[i]);
		if (it != objects.end())
			objects.erase(it);
	}

	externalObjects.clear();

	for (int i = 0; i < objects.size(); ++i)
		if (!objects[i]->getParentObject())
			delete objects[i];
	objects.clear();
}