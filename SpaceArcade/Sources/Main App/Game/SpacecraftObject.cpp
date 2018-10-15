#include "SpacecraftObject.h"
#include "GameLevel.h"

SpacecraftObject::SpacecraftObject()
{
	laser_ray = new GameObject();
}

SpacecraftObject::~SpacecraftObject()
{
	clear();
}

void SpacecraftObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity)
{
	GameObject::init(level, pos, size, sprite, velocity);
}

void SpacecraftObject::update(float delta)
{
	for(int i = 0; i < lasers_poses.size(); ++i)
	{
		if(lasers_poses[i].y + laser_ray->Size.y > 0)
			lasers_poses[i].y -= delta * laser_ray->Velocity.y;
		else
		{
			lasers_poses.erase(lasers_poses.begin() + i);
			i--;
		}
	}
}

void SpacecraftObject::Draw()
{
	for (int i = 0; i < lasers_poses.size(); ++i)
	{
		laser_ray->Position = lasers_poses[i];
		laser_ray->Draw();
	}
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

	for (int i = 0; i < lasers_poses.size(); ++i)
		lasers_poses[i] = glm::vec2(lasers_poses[i].x * screenRatio.x, lasers_poses[i].y * screenRatio.y);
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
		lasers_poses.push_back(glm::vec2(Position.x + Size.x / 2 - laser_ray->Size.x / 2, Position.y));
		*key_pressed = true;
	}
}

GameObject* SpacecraftObject::getLaserRay()
{
	return laser_ray;
}

void SpacecraftObject::clear()
{
	if (laser_ray)
	{
		delete laser_ray;
		laser_ray = NULL;
	}
}