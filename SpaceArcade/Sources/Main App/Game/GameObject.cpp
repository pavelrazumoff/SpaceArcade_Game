#include "GameObject.h"
#include "GameLevel.h"

GameObject::GameObject()
	: Position(0, 0), Size(1, 1), Velocity(0.0f), Rotation(0.0f)
{ 

}

GameObject::~GameObject()
{
	clear();
}

void GameObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Velocity = velocity;
	this->pLevel = level;
}

void GameObject::update(float delta)
{
	
}

void GameObject::Draw()
{
	if(visible)
		pLevel->getRenderer()->DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation);
}

void GameObject::resize()
{
}

void GameObject::setVisible(bool visible)
{
	this->visible = visible;
}

void GameObject::clear()
{
	
}