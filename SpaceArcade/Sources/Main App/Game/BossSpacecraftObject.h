#pragma once
/*
	BossSpacecraftObject.h
	This class represents improved spacecraft object class.
	It can spawn many lasers in one time. Plus it has opportunity to
	enable energy shield, that can be destroyed only by specific object types
	such as rockets or ion charges.
*/

#include "SpacecraftObject.h"

class BossSpacecraftObject : public SpacecraftObject
{
public:
	BossSpacecraftObject();
	BossSpacecraftObject(bool createChildren);
	virtual ~BossSpacecraftObject();

	virtual void init(GameLevel* level, glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void resize();
	virtual void clear();

	virtual GameObject* clone();
	virtual void cloneParams(GameObject* obj);

	virtual void draw(bool useInstanced = false, int amount = 0);
	virtual void update(float delta);

	virtual bool notify(GameObject* notifiedObject, NotifyCode code);

	virtual void makeReaction(glm::vec2 difference, GameObject* otherObj, bool collisionChecker);

	void spawnLaserRays();
	void enableShield(bool enable);
	bool isShieldEnabled();

	void setEnergyShield(GameObject* shield);
	GameObject* getEnergyShield();

	void addLaserStartPoint(glm::vec2 pos);
	void setIndexOfPreferredLaserPoint(int index);

protected:
	GameObject* pEnergyShieldTemplate = NULL;
	GameObject* energyShield = NULL;

	// relative laser starting positions.
	std::vector<glm::vec2>	lasersStartPoints;
	// this indicates, what laser starting point to use when there is a few energy
	// to produce all laser rays in current time.
	int indexOfPrefferedLaserPoint = 0;
};