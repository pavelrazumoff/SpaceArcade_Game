#include "SpacecraftObject.h"
#include "GameLevel.h"

SpacecraftObject::SpacecraftObject()
{
	damage = 30.0f;

	laser_ray = new GameObject();
	laser_ray->setIsDamagingObject(true);
	laser_ray->setDamage(10.0f);
	laser_ray->setHealth(1.0f); // destroys momentally.
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
	newObject->Rotation = this->Rotation;
	newObject->setLaserRay(this->getLaserRay()->clone());

	return newObject;
}

void SpacecraftObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void SpacecraftObject::update(float delta)
{
	for(int i = 0; i < laser_rays.size(); ++i)
	{
		if(laser_rays[i]->Position.y + laser_rays[i]->Size.y > 0)
			laser_rays[i]->Position.y -= delta * laser_rays[i]->Velocity.y;
		else
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
	glm::vec4 indents = pLevel->getScreenIndents();
	glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 initialScreenRatio = dimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	glm::vec2 shift = glm::vec2(delta * Velocity.x * initialScreenRatio.x, delta * Velocity.y * initialScreenRatio.y);

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
		laser_rays.push_back(laser_ray->clone());
		laser_rays.back()->Position = glm::vec2(Position.x + Size.x / 2 - laser_ray->Size.x / 2, Position.y);
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

void SpacecraftObject::setLaserRay(GameObject* laser)
{
	if (laser_ray)
		delete laser_ray;
	laser_ray = laser;
}

GameObject* SpacecraftObject::getLaserRay()
{
	return laser_ray;
}

void SpacecraftObject::clear()
{
	for (int i = 0; i < laser_rays.size(); ++i)
		delete laser_rays[i];

	laser_rays.clear();

	if (laser_ray)
	{
		delete laser_ray;
		laser_ray = NULL;
	}
}