#include "GameObject.h"
#include "GameLevel.h"
#include "AI/AIController.h"
#include "AI/LevelBehaviour.h"

GameObject::GameObject()
	: Position(0, 0), Size(1, 1), Velocity(0.0f), InitialRotation(0.0f), Rotation(0.0f)
{ 

}

GameObject::GameObject(bool createChildren)
	: Position(0, 0), Size(1, 1), Velocity(0.0f), InitialRotation(0.0f), Rotation(0.0f)
{
	
}

GameObject::~GameObject()
{
	clear();
}

GameObject* GameObject::clone()
{
	GameObject* newObject = new GameObject();
	this->cloneParams(newObject);
	newObject->init(this->pLevel, this->Position, this->Size, this->Sprite, this->Velocity);

	return newObject;
}

void GameObject::cloneParams(GameObject* obj)
{
	obj->RelativePosition = this->RelativePosition;
	obj->InitialRotation = this->InitialRotation;
	obj->Rotation = this->Rotation;
	obj->setObjectType(this->getObjectType());
	obj->setIsDamagingObject(this->isDamagingObject());
	obj->setDamage(this->getDamage());
	obj->setHealth(this->getHealth());
	obj->setMaxHealth(this->getMaxHealth());
	obj->setParentObject(this->getParentObject());
	obj->setExplosionTime(this->explosionTime);
	obj->setExplosionSprite(this->ExplosionSprite);
	obj->setSelfDestroyTime(this->selfDestroyTime);
	obj->startSelfDestroying(this->selfDestroying);
	obj->setNonPlayerObject(this->nonPlayerObject);
	obj->setUsePhysics(this->isUsePhysics());
	obj->setCollisionCheck(this->collisionCheck);
	obj->setAIController(this->getAIController());
	obj->setControlVelocityByRotation(this->isControlVelocityByRotation());
	obj->setUseAnimation(this->isUseAnimation());
	obj->setUseBackAndForthAnimation(this->useBackAndForthAnim);
	obj->setAnimationOrder(this->animationOrder);
	obj->setAnimationDuration(this->getAnimationDuration());
	obj->setDamageAsAttachment(this->isDamageAsAttachment());
	obj->setImpulseFactor(this->impulseFactor);
	obj->setScoreContribution(this->scoreContribution);
}

void GameObject::init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity, bool addToLevel)
{
	this->Position = pos;
	this->Size = size;
	this->Velocity = velocity;
	this->pLevel = level;

	this->setSprite(sprite);

	if(addToLevel && level)
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
			if (currentExplosionFrame >= ExplosionSprite->numOfFrames)
				currentExplosionFrame = ExplosionSprite->numOfFrames - 1;
		}
		if (explosionTime <= 0.0f)
			setReadyForDeath(true);
	}

	if (health > 0.0f)
	{
		updateAnimation(delta);

		if (selfDestroying && selfDestroyTime > 0.0f)
		{
			selfDestroyTime -= delta;
			if (selfDestroyTime <= 0.0f)
				setHealth(0.0f);
		}

		InitialRotation += Rotation * delta;

		glm::vec2 currentVelocity = Velocity;

		if (controlVelocityByRot)
		{
			glm::mat4 model;
			model = glm::rotate(model, glm::radians(InitialRotation), glm::vec3(0.0f, 0.0f, 1.0f));
			currentVelocity = model * glm::vec4(currentVelocity, 0.0f, 1.0f);
		}

		Position.x += currentVelocity.x * delta;
		Position.y += currentVelocity.y * delta;

		if (InitialRotation >= 360.0f || InitialRotation <= -360.0f)
			InitialRotation = 0.0f;

		if (!nonPlayerObject)
		{
			float restrZone = pLevel->getRenderer()->getCurrentScreenDimensions().y - pLevel->getPlayerRestrictionHeight();
			if (Position.y < restrZone)
				Position.y = restrZone;
		}
		else
		{
			// when object goes out of screen bounds when it already overstepped top side of the screen, destroy it.
			glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
			if (Position.y > 0.0f && (Position.x + Size.x < 0.0f || Position.x > screenDimensions.x || Position.y > screenDimensions.y))
				setHealth(0.0f);
		}
	}

	// Update AI after all.
	if(aiController)
		aiController->update(delta);

	// update model (world) matrix after all calculations.
	updateModelMatrix();

	for (int i = 0; i < attachedObjects.size(); ++i)
	{
		attachedObjects[i]->Position = this->Position + attachedObjects[i]->RelativePosition;
		attachedObjects[i]->update(delta);
	}
}

void GameObject::updatePaused(float delta)
{
	updateModelMatrix();

	for (int i = 0; i < attachedObjects.size(); ++i)
	{
		attachedObjects[i]->Position = this->Position + attachedObjects[i]->RelativePosition;
		attachedObjects[i]->updatePaused(delta);
	}
}

void GameObject::updateModelMatrix()
{
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(Position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * Size.x, 0.5f * Size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, glm::radians(this->InitialRotation), glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * Size.x, -0.5f * Size.y, 0.0f)); // Move origin back

	if (health <= 0.0f)
		explosionModel = glm::scale(model, glm::vec3(Size.x, Size.x, 1.0f));		// make it square.

	model = glm::scale(model, glm::vec3(Size, 1.0f)); // Last scale
}

void GameObject::updateAnimation(float delta)
{
	if (!useAnimation)
		return;
	
	// if animation order is forward playing.
	if (animationOrder)
	{
		if (animationTime > 0.0f)
			animationTime -= delta;
		else
		{
			if (!useBackAndForthAnim)
				animationTime = animationDuration;
			else
			{
				animationTime = 0.0f;
				animationOrder = !animationOrder;
			}
		}
	}
	else
	{
		// animation order is backward playing.
		if (animationTime < animationDuration)
			animationTime += delta;
		else
		{
			if (!useBackAndForthAnim)
				animationTime = 0.0f;
			else
			{
				animationTime = animationDuration;
				animationOrder = !animationOrder;
			}
		}
	}

	currentAnimationFrame = Sprite->numOfFrames - animationTime / animationTimeStep;
	if (currentAnimationFrame >= Sprite->numOfFrames)
		currentAnimationFrame = Sprite->numOfFrames - 1;
}

void GameObject::draw(bool useInstanced, int amount)
{
	// prepare renderer for drawing instances, if it has to.
	pLevel->getRenderer()->setUseInstanced(useInstanced, amount);

	for (int i = 0; i < attachedOnBottom.size(); ++i)
		attachedOnBottom[i]->draw(useInstanced, amount);

	if(visible && health > 0.0f)
		pLevel->getRenderer()->DrawSprite(this->Sprite, this->model, glm::vec4(0.0f), currentAnimationFrame);
	if(health <= 0.0f && explosionTime > 0.0f && ExplosionSprite->ID >= 0)
		pLevel->getRenderer()->DrawSprite(this->ExplosionSprite, this->explosionModel, glm::vec4(0.0f), currentExplosionFrame);

	for (int i = 0; i < attachedOnTop.size(); ++i)
		attachedOnTop[i]->draw(useInstanced, amount);
	
	// drop renderer from drawing instances.
	pLevel->getRenderer()->dropInstanced();
}

void GameObject::resize()
{
	glm::vec2 screenRatio = pLevel->getRenderer()->getScreenRatio();

	Position = glm::vec2(Position.x * screenRatio.x, Position.y * screenRatio.y);
	Velocity = glm::vec2(Velocity.x * screenRatio.x, Velocity.y * screenRatio.y);

	for (int i = 0; i < attachedObjects.size(); ++i)
		attachedObjects[i]->resize();
}

void GameObject::handleInput(GLFWwindow *window, float delta)
{
}

void GameObject::processKey(int key, int action, bool* key_pressed)
{
}

bool GameObject::checkCollision(GameObject* obj, glm::vec2& difference)
{
	if (this->parentObject == obj || obj->getParentObject() == this)
	{
		bool attachedFirst = this->isObjectAttachedTo(obj);
		bool attachedSecond = obj->isObjectAttachedTo(this);

		if (attachedFirst && !obj->isDamageAsAttachment())
			return false;
		if(attachedSecond && !this->isDamageAsAttachment())
			return false;

		if (!attachedFirst && !attachedSecond)
			return false;
	}
	else
		if (this->parentObject && this->parentObject == obj->getParentObject())
			return false;

	// Collision x-axis?
	bool collisionX = this->Position.x + this->Size.x >= obj->Position.x &&
		obj->Position.x + obj->Size.x >= this->Position.x;
	// Collision y-axis?
	bool collisionY = this->Position.y + this->Size.y >= obj->Position.y &&
		obj->Position.y + obj->Size.y >= this->Position.y;

	glm::vec2 center(this->Position + glm::vec2(Size.x / 2, Size.y / 2));
	glm::vec2 center2(obj->Position + glm::vec2(obj->Size.x / 2, obj->Size.y / 2));
	// Get difference vector between both centers
	difference = center - center2;

	// Collision only if on both axes
	return collisionX && collisionY;
}

void GameObject::makeCollision(GameObject* obj)
{
	if (this->readyForDeath || obj->getReadyForDeath() ||
		!pLevel->getBehaviour()->checkForCollisionAddiction(this, obj))
		return;

	obj->setHealth(obj->getHealth() - this->damage);

	GameObject* pPlayerObject = pLevel->getBehaviour()->getPlayerObject();
	if (!pPlayerObject)
		return;

	if (obj->getHealth() <= 0.0f && (this == pPlayerObject || this->getParentObject() == pPlayerObject))
		pLevel->addScore(obj->getScoreContribution());
}

void GameObject::makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker)
{
	if (health <= 0.0f && explosionTime <= 0.0f)
	{
		if (!readyForDeath)
			setReadyForDeath(true);
		return;
	}
}

void GameObject::attachNewObject(GameObject* obj, bool onTop)
{
	auto it = find(attachedObjects.begin(), attachedObjects.end(), obj);
	if (it == attachedObjects.end())
	{
		attachedObjects.push_back(obj);
		obj->setParentObject(this);

		if (onTop)
			attachedOnTop.push_back(obj);
		else
			attachedOnBottom.push_back(obj);
	}
}

void GameObject::removeAttachedObject(GameObject* obj)
{
	auto it = find(attachedObjects.begin(), attachedObjects.end(), obj);
	if (it != attachedObjects.end())
		attachedObjects.erase(it);

	it = find(attachedOnTop.begin(), attachedOnTop.end(), obj);
	if (it != attachedOnTop.end())
		attachedOnTop.erase(it);

	it = find(attachedOnBottom.begin(), attachedOnBottom.end(), obj);
	if (it != attachedOnBottom.end())
		attachedOnBottom.erase(it);
}

void GameObject::addPostDeathObject(GameObject* obj)
{
	auto it = find(postDeathObjects.begin(), postDeathObjects.end(), obj);
	if (it == postDeathObjects.end())
	{
		postDeathObjects.push_back(obj);
		obj->hideFromLevel(true);
	}
}

int GameObject::getAttachedObjectsSize()
{
	return attachedObjects.size();
}

int GameObject::getAttachedObjectsSizeByType(int type)
{
	int size = 0;
	for (int i = 0; i < attachedObjects.size(); ++i)
		if (attachedObjects[i]->getObjectType() == type)
			size++;
	return size;
}

int GameObject::getPostDeathObjectsSize()
{
	return postDeathObjects.size();
}

GameObject* GameObject::getAttachedObjectByIndex(int index)
{
	if (index < 0 || index >= attachedObjects.size())
		return NULL;
	return attachedObjects[index];
}

GameObject* GameObject::getAttachedObjectByTypeIndex(int type, int index)
{
	for(int i = 0, k = 0; i < attachedObjects.size(); ++i)
		if (attachedObjects[i]->getObjectType() == type)
		{
			if (k == index)
				return attachedObjects[i];
			k++;
		}

	return NULL;
}

GameObject* GameObject::getPostDeathObjectByIndex(int index)
{
	if (index < 0 || index >= postDeathObjects.size())
		return NULL;
	return postDeathObjects[index];
}

bool GameObject::isObjectAttachedTo(GameObject* obj)
{
	auto it = find(attachedObjects.begin(), attachedObjects.end(), obj);
	if (it != attachedObjects.end())
		return true;
	return false;
}

bool GameObject::isOffTheScreen(glm::vec2 screenDimensions)
{
	if (Position.x + Size.x < 0.0f || Position.y + Size.y < 0.0f ||
		Position.x > screenDimensions.x || Position.y > screenDimensions.y)
		return true;
	return false;
}

void GameObject::setParentObject(GameObject* parent)
{
	parentObject = parent;
}

GameObject* GameObject::getParentObject()
{
	return parentObject;
}

bool GameObject::notify(GameObject* notifiedObject, NotifyCode code)
{
	auto it = find(attachedObjects.begin(), attachedObjects.end(), notifiedObject);
	if (it == attachedObjects.end())
		return false;

	switch (code)
	{
	case Destroyed:
	{
		pLevel->removeObject(notifiedObject);
		attachedObjects.erase(it);

		it = find(attachedOnTop.begin(), attachedOnTop.end(), notifiedObject);
		if (it != attachedOnTop.end())
			attachedOnTop.erase(it);

		it = find(attachedOnBottom.begin(), attachedOnBottom.end(), notifiedObject);
		if (it != attachedOnBottom.end())
			attachedOnBottom.erase(it);

		delete notifiedObject;

		return true;
	}
	break;
	default:
		break;
	}

	return false;
}

void GameObject::setExplosionSprite(Texture2D* sprite)
{
	ExplosionSprite = sprite;
	if (sprite)
		explosionTimeStep = explosionTime / sprite->numOfFrames;
}

void GameObject::setHealthChangedCallback(void(*actionCallback)(float, float))
{
	healthChanged = actionCallback;
}

void GameObject::setExplosionSoundName(std::string name)
{
	explosionSoundName = name;
}

void GameObject::setLevel(GameLevel* level)
{
	pLevel = level;
	if(level)
		pLevel->addNewObject(this);
}

void GameObject::setObjectType(int type)
{
	objectType = type;
}

void GameObject::setSprite(Texture2D* sprite)
{
	Sprite = sprite;
	if (Sprite)
		animationTimeStep = animationDuration / Sprite->numOfFrames;
}

void GameObject::setVisible(bool visible)
{
	this->visible = visible;
}

void GameObject::hideFromLevel(bool hide)
{
	hidden = hide;
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
	if (hp > maxHealth)
		health = maxHealth;
	else
		health = hp;

	if (health <= 0.0f)
	{
		if (explosionSoundName.compare("") &&
			!isOffTheScreen(pLevel->getRenderer()->getCurrentScreenDimensions()))
			pLevel->playSound(explosionSoundName, false);

		for (int i = 0; i < attachedObjects.size(); ++i)
			attachedObjects[i]->setVisible(false);
	}

	if (healthChanged)
		healthChanged(this->health, this->maxHealth);
}

void GameObject::setMaxHealth(float hp)
{
	maxHealth = hp;
}

void GameObject::setReadyForDeath(bool ready)
{
	readyForDeath = ready;
	// here we have to be able to spawn some object before death.
	for (int i = 0; i < postDeathObjects.size(); ++i)
	{
		postDeathObjects[i]->hideFromLevel(false);
		postDeathObjects[i]->Position = this->Position;
	}
}

void GameObject::setExplosionTime(float time)
{
	explosionTime = time;
}

void GameObject::setSelfDestroyTime(float time)
{
	selfDestroyTime = time;
}

void GameObject::startSelfDestroying(bool start)
{
	selfDestroying = start;
}

void GameObject::setNonPlayerObject(bool nonPlayer)
{
	nonPlayerObject = nonPlayer;
}

void GameObject::setUsePhysics(bool physics)
{
	usePhysics = physics;
}

void GameObject::setCollisionCheck(bool check)
{
	collisionCheck = check;
}

void GameObject::setAIController(AIController* controller)
{
	aiController = controller;
	if(controller)
		controller->setPawn(this);
}

void GameObject::setControlVelocityByRotation(bool control)
{
	controlVelocityByRot = control;
}

void GameObject::setDamageAsAttachment(bool damage)
{
	damageAsAttachment = damage;
}

void GameObject::setUseAnimation(bool animation)
{
	useAnimation = animation;
}

void GameObject::setUseBackAndForthAnimation(bool backForth)
{
	useBackAndForthAnim = backForth;
}

void GameObject::setAnimationOrder(bool order)
{
	animationOrder = order;
}

void GameObject::setAnimationDuration(float duration)
{
	animationDuration = duration;
	animationTime = duration;

	if (Sprite)
		animationTimeStep = animationDuration / Sprite->numOfFrames;
}

void GameObject::setImpulseFactor(float impulse)
{
	impulseFactor = impulse;
}

void GameObject::applyImpulse(float impulse)
{
	appliedImpulse = impulse;
}

void GameObject::setScoreContribution(int score)
{
	scoreContribution = score;
}

GameLevel* GameObject::getLevel()
{
	return pLevel;
}

glm::mat4 GameObject::getModel()
{
	return model;
}

int GameObject::getObjectType()
{
	return objectType;
}

bool GameObject::isVisible()
{
	return visible;
}

bool GameObject::isHiddenFromLevel()
{
	return hidden;
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

float GameObject::getMaxHealth()
{
	return maxHealth;
}

bool GameObject::getReadyForDeath()
{
	return readyForDeath;
}

float GameObject::getExplosionTime()
{
	return explosionTime;
}

bool GameObject::isNonPlayerObject()
{
	return nonPlayerObject;
}

bool GameObject::isUsePhysics()
{
	return usePhysics;
}

bool GameObject::isCheckingCollision()
{
	return collisionCheck;
}

AIController* GameObject::getAIController()
{
	return aiController;
}

bool GameObject::isControlVelocityByRotation()
{
	return controlVelocityByRot;
}

bool GameObject::isDamageAsAttachment()
{
	return damageAsAttachment;
}

bool GameObject::isUseAnimation()
{
	return useAnimation;
}

bool GameObject::isUseBackAndForthAnimation()
{
	return useBackAndForthAnim;
}

bool GameObject::getAnimationOrder()
{
	return animationOrder;
}

float GameObject::getAnimationDuration()
{
	return animationDuration;
}

float GameObject::getImpulseFactor()
{
	return impulseFactor;
}

int GameObject::getScoreContribution()
{
	return scoreContribution;
}

void GameObject::clear()
{
	if (aiController)
	{
		aiController->BlockAI();
		aiController->setPawn(NULL);
	}

	// disconnect attached objects from this object and let them to self-destruct manually through level.
	for (int i = 0; i < attachedObjects.size(); ++i)
	{
		attachedObjects[i]->setParentObject(NULL);
		attachedObjects[i]->setReadyForDeath(true);
	}

	attachedObjects.clear();
	attachedOnTop.clear();
	attachedOnBottom.clear();
}