#include "GameObject.h"
#include "GameLevel.h"

GameObject::GameObject()
	: Position(0, 0), Size(1, 1), Velocity(0.0f), Rotation(0.0f)
{ 

}

void GameObject::init(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Velocity = velocity;
}

void GameObject::Draw()
{
	pLevel->getRenderer()->DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation);
}

void GameObject::resize()
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
}

void GameObject::handleInput(GLFWwindow *window, float delta)
{
	glm::vec4 indents = pLevel->getScreenIndents();
	glm::vec2 dimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 initialScreenRatio = dimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	glm::vec2 shift = glm::vec2(delta * Velocity.x * initialScreenRatio.x, delta * Velocity.y * initialScreenRatio.y);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if(Position.y - shift.y > indents.y)
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

void GameObject::setLevelPointer(GameLevel* level)
{
	pLevel = level;
}