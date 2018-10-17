#include "GameObject.h"
#include "GameLevel.h"

GameObject::GameObject()
	: Position(0, 0), Size(1, 1), Velocity(0.0f), Rotation(0.0f)
{ 

}

GameObject::GameObject(bool createChildren)
	: Position(0, 0), Size(1, 1), Velocity(0.0f), Rotation(0.0f)
{
	
}

GameObject::~GameObject()
{
	clear();
}

GameObject* GameObject::clone()
{
	GameObject* newObject = new GameObject();
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);
	newObject->Rotation = this->Rotation;
	newObject->setIsDamagingObject(this->isDamagingObject());
	newObject->setDamage(this->getDamage());
	newObject->setHealth(this->getHealth());
	newObject->setParentObject(this->getParentObject());

	return newObject;
}

void GameObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity, bool addToLevel)
{
	this->Position = pos;
	this->Size = size;
	this->Sprite = sprite;
	this->Velocity = velocity;
	this->pLevel = level;

	if(addToLevel)
		level->addNewObject(this);
}

void GameObject::update(float delta)
{
	if (health <= 0.0f)
	{
		if (explosionTime > 0.0f)
		{
			explosionTime -= delta;
			currentExplosionFrame = ExplosionSprite->numOfFrames - explosionTime / explosionTimeStep;
		}
		if (explosionTime <= 0.0f)
			readyForDeath = true;
	}
}

void GameObject::Draw()
{
	if(visible && health > 0.0f)
		pLevel->getRenderer()->DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation);
	if(health <= 0.0f && explosionTime > 0.0f && ExplosionSprite->ID >= 0)
		pLevel->getRenderer()->DrawSprite(this->ExplosionSprite, this->Position, this->Size, this->Rotation, currentExplosionFrame);
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
}

void GameObject::processKey(int key, int action, bool* key_pressed)
{
}

bool GameObject::checkCollision(GameObject* obj)
{
	if (this->parentObject == obj || obj->getParentObject() == this)
		return false;

	// Collision x-axis?
	bool collisionX = this->Position.x + this->Size.x >= obj->Position.x &&
		obj->Position.x + obj->Size.x >= this->Position.x;
	// Collision y-axis?
	bool collisionY = this->Position.y + this->Size.y >= obj->Position.y &&
		obj->Position.y + obj->Size.y >= this->Position.y;
	// Collision only if on both axes
	return collisionX && collisionY;
}

void GameObject::makeCollision(GameObject* obj)
{
	if (this->readyForDeath || obj->getReadyForDeath())
		return;

	this->health -= obj->getDamage();
	obj->setHealth(obj->getHealth() - this->damage);
}

void GameObject::makeReaction()
{
	if (health <= 0.0f && explosionTime <= 0.0f)
		readyForDeath = true;
}

void GameObject::setParentObject(GameObject* parent)
{
	parentObject = parent;
}

GameObject* GameObject::getParentObject()
{
	return parentObject;
}

void GameObject::notify(GameObject* notifiedObject, NotifyCode code)
{
}

void GameObject::setExplosionSprite(Texture2D* sprite)
{
	ExplosionSprite = sprite;
	if (sprite)
		explosionTimeStep = explosionTime / sprite->numOfFrames;
}

void GameObject::setVisible(bool visible)
{
	this->visible = visible;
}

void GameObject::setIsDamagingObject(bool damaging)
{
	damagingObject = damaging;
}

void GameObject::setDamage(float damage)
{
	this->damage = damage;
}

void GameObject::setHealth(float hp)
{
	health = hp;
}

void GameObject::setExplosionTime(float time)
{
	explosionTime = time;
}

bool GameObject::isVisible()
{
	return visible;
}

bool GameObject::isDamagingObject()
{
	return damagingObject;
}

float GameObject::getDamage()
{
	return damage;
}

float GameObject::getHealth()
{
	return health;
}

bool GameObject::getReadyForDeath()
{
	return readyForDeath;
}

float GameObject::getExplosionTime()
{
	return explosionTime;
}

void GameObject::clear()
{
	
}