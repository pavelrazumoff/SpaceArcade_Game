#include "SpacecraftObject.h"
#include "GameLevel.h"

SpacecraftObject::SpacecraftObject()
{
	damage = 10.0f;
	explosionTime = 1.5f;
	usePhysics = true;

	laser_ray = new GameObject();
	laser_ray->setIsDamagingObject(true);
	laser_ray->setDamage(20.0f);
	laser_ray->setHealth(1.0f); // destroys momentally.
	laser_ray->setUsePhysics(false);
	laser_ray->setParentObject(this);
}

SpacecraftObject::SpacecraftObject(bool createChildren)
{
	if(createChildren)
		laser_ray = new GameObject();
}

SpacecraftObject::~SpacecraftObject()
{
	clear();
}

GameObject* SpacecraftObject::clone()
{
	SpacecraftObject* newObject = new SpacecraftObject(false);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);
	this->cloneParams(newObject);

	return newObject;
}

void SpacecraftObject::cloneParams(GameObject* obj)
{
	GameObject::cloneParams(obj);

	SpacecraftObject* spacecraft = (SpacecraftObject*)obj;
	spacecraft->setLaserRay(this->getLaserRay()->clone());
	spacecraft->setTargetEnemy(this->targetEnemy);
	spacecraft->setFireFrequency(this->fireFrequency);
}

void SpacecraftObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void SpacecraftObject::update(float delta)
{
	GameObject::update(delta);
	if (health > 0.0f && (abs(Velocity.x) > 0 || abs(Velocity.y) > 0))
	{
		if (abs(Velocity.x) > 0)
			Velocity.x = glm::sign(Velocity.x) * (abs(Velocity.x) - (impulseFactor * 1.5) * delta);
		if (abs(Velocity.y) > 0)
			Velocity.y = glm::sign(Velocity.y) * (abs(Velocity.y) - (impulseFactor * 1.5) * delta);

		if (abs(Velocity.x) < 10)
			Velocity.x = 0;
		if (abs(Velocity.y) < 10)
			Velocity.y = 0;
	}

	if (controlledByAI && health > 0.0f)
	{
		followTargetEnemy(delta);

		if (readyToFire)
		{
			timeToFire -= delta;
			if (timeToFire <= 0.0f)
			{
				spawnLaserRay();
				timeToFire = fireFrequency;
			}
		}
	}

	for(int i = 0; i < laser_rays.size(); ++i)
	{
		laser_rays[i]->InitialRotation = this->InitialRotation;
		laser_rays[i]->update(delta);
		if(laser_rays[i]->isOffTheScreen(pLevel->getRenderer()->getCurrentScreenDimensions()))
		{
			GameObject* laser = laser_rays[i];
			pLevel->removeObject(laser);
			delete laser;
			laser_rays.erase(laser_rays.begin() + i);
			i--;
		}
	}
}

void SpacecraftObject::Draw()
{
	for (int i = 0; i < laser_rays.size(); ++i)
		laser_rays[i]->Draw();
	GameObject::Draw();
}

void SpacecraftObject::resize()
{
	glm::vec4 indents = pLevel->getScreenIndents();

	glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = pLevel->getRenderer()->getScreenRatio();

	Position = glm::vec2(Position.x * screenRatio.x, Position.y * screenRatio.y);

	if (Position.x < indents.x)
		Position.x = indents.x;
	if (Position.x + Size.x > dimensions.x - indents.x)
		Position.x = dimensions.x - indents.x - Size.x;
	if (Position.y < indents.y)
		Position.y = indents.y;
	if (Position.y + Size.y > dimensions.y - indents.y)
		Position.y = dimensions.y - indents.y - Size.y;

	for (int i = 0; i < laser_rays.size(); ++i)
		laser_rays[i]->Position = glm::vec2(laser_rays[i]->Position.x * screenRatio.x, laser_rays[i]->Position.y * screenRatio.y);
}

void SpacecraftObject::handleInput(GLFWwindow *window, float delta)
{
	if (health <= 0.0f || abs(Velocity.x) > (impulseFactor / 2) || abs(Velocity.y) > (impulseFactor / 2))
		return;

	glm::vec4 indents = pLevel->getScreenIndents();
	glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 initialScreenRatio = dimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	glm::vec2 shift = glm::vec2(delta * VelocityScale.x * initialScreenRatio.x, delta * VelocityScale.y * initialScreenRatio.y);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (Position.y - shift.y > indents.y)
			Position.y -= shift.y;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (Position.y + shift.y + Size.y < dimensions.y - indents.y)
			Position.y += shift.y;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (Position.x - shift.x > indents.x)
			Position.x -= shift.x;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if (Position.x + Size.x + shift.x < dimensions.x - indents.x)
			Position.x += shift.x;
	}
}

void SpacecraftObject::processKey(int key, int action, bool* key_pressed)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && *key_pressed == false)
	{
		spawnLaserRay();
		*key_pressed = true;
	}
}

void SpacecraftObject::notify(GameObject* notifiedObject, NotifyCode code)
{
	std::vector<GameObject*>::iterator it;

	it = find(laser_rays.begin(), laser_rays.end(), notifiedObject);
	if (it == laser_rays.end())
		return;

	switch (code)
	{
	case Destroyed:
	{
		pLevel->removeObject(notifiedObject);
		laser_rays.erase(it);
		delete notifiedObject;
	}
		break;
	default:
		break;
	}
}

void SpacecraftObject::setTargetEnemy(GameObject* target)
{
	targetEnemy = target;
}

void SpacecraftObject::setLaserRay(GameObject* laser)
{
	if (laser_ray)
		delete laser_ray;
	laser_ray = laser;
}

GameObject* SpacecraftObject::getTargetEnemy()
{
	return targetEnemy;
}

GameObject* SpacecraftObject::getLaserRay()
{
	return laser_ray;
}

void SpacecraftObject::setControlVelocityByRotation(bool control)
{
	GameObject::setControlVelocityByRotation(control);
	laser_ray->setControlVelocityByRotation(control);
}

void SpacecraftObject::setFireFrequency(float freq)
{
	fireFrequency = freq;
	timeToFire = freq;
}

float SpacecraftObject::getFireFrequency()
{
	return fireFrequency;
}

void SpacecraftObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	GameObject::makeReaction(difference, otherObj, collisionChecker);

	if (!otherObj->isUsePhysics())
		return;

	// Collision resolution
	glm::vec2 normalizedDiff = glm::normalize(difference);
	if (!collisionChecker)
		normalizedDiff = -normalizedDiff;
	
	if (controlVelocityByRot)
	{
		glm::mat4 model;
		model = glm::rotate(model, glm::radians(InitialRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		normalizedDiff = model * glm::vec4(normalizedDiff, 0.0f, 1.0f);
	}
	Velocity = normalizedDiff * glm::vec2(impulseFactor);
}

void SpacecraftObject::spawnLaserRay()
{
	if (!laser_ray || !laser_ray->getLevel())
		return;
	laser_rays.push_back(laser_ray->clone());
	laser_rays.back()->Position = glm::vec2(Position.x + Size.x / 2 - laser_ray->Size.x / 2, Position.y);
	laser_rays.back()->InitialRotation = this->InitialRotation;
}

void SpacecraftObject::followTargetEnemy(float delta)
{
	if (!targetEnemy)
		return;

	if (targetEnemy->getHealth() <= 0.0f)
	{
		targetEnemy = NULL;
		readyToFire = false;
		return;
	}

	if (abs(Position.x - targetEnemy->Position.x) > 20.0f)
	{
		glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
		glm::vec2 initialScreenRatio = dimensions / pLevel->getRenderer()->getInitialScreenDimensions();

		glm::vec2 shift = glm::vec2(delta * VelocityScale.x * initialScreenRatio.x, delta * VelocityScale.y * initialScreenRatio.y);

		if (Position.x < targetEnemy->Position.x)
			Position.x += shift.x;
		else
			Position.x -= shift.x;

		readyToFire = false;
	}
	else
	{
		if (!readyToFire)
			timeToFire = 0.0f;
		readyToFire = true;
	}

	if (abs(Position.x - targetEnemy->Position.x) < 5.0f)
		Velocity.x = 0.0f;
}

void SpacecraftObject::clear()
{
	for (int i = 0; i < laser_rays.size(); ++i)
		laser_rays[i]->setParentObject(NULL);

	laser_rays.clear();

	if (laser_ray)
	{
		delete laser_ray;
		laser_ray = NULL;
	}
}