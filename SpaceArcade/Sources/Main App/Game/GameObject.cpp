#include "GameObject.h"

GameObject::GameObject()
	: SourcePosition(0, 0), Position(0, 0), Size(1, 1), Velocity(0.0f), Rotation(0.0f)
{ 

}

void GameObject::init(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity)
{
	this->SourcePosition = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Velocity = velocity;

	this->Position = glm::vec2(0.0f, 0.0f);
}

void GameObject::Draw(SpriteRenderer &renderer, glm::vec2 screenRatio)
{
	glm::vec2 currentPos = glm::vec2(SourcePosition.x * screenRatio.x, SourcePosition.y * screenRatio.y);

	currentPos += glm::vec2(Position.x * screenRatio.x, Position.y * screenRatio.y);

	renderer.DrawSprite(this->Sprite, currentPos, this->Size, this->Rotation);
}

void GameObject::handleInput(GLFWwindow *window, float delta)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Position.y -= delta * Velocity.y;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Position.y += delta * Velocity.y;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Position.x -= delta * Velocity.x;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Position.x += delta * Velocity.x;
}