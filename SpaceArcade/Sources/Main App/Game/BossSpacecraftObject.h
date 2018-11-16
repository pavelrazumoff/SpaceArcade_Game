#pragma once
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
	int indexOfPrefferedLaserPoint = 0;
};