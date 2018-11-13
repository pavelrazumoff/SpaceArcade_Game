#pragma once
/*
	StartLevelBehaviour.h
	This class is derived from LevelBehaviour. It presents basic level
	with alternating level modes one by one. Base level objects here 
	are playerCraft, enemyCraft and meteorites.
*/

#include "LevelBehaviour.h"

enum StartLevelMode
{
	Introducing = 0,
	MeteorFighting,
	SpaceCraftEnemyIntroducing,
	SpaceCraftEnemyFighting,
	EnergyBarriersShowing,
	TeamCraftEnemyIntroducing,
	TeamCraftEnemyFighting,
	BossSpaceCraftIntroducing,
	BossSpaceCraftFighting,
	BossSpaceCraftLeaving,
	End
};

class StartLevelBehaviour : public LevelBehaviour
{
public:
	StartLevelBehaviour();
	StartLevelBehaviour(GameLevel* level, ResourceManager* resManager);
	virtual ~StartLevelBehaviour();

	virtual void init();
	virtual void update(float delta);
	virtual void startBehaviour();
	virtual void pauseBehaviour();
	virtual void resumeBehaviour();

	virtual void clear();

	// only one of these functions is called in one time.
	// they are handling current level mode (sublevel) and when the time comes
	// change level mode to the next.
	void updateIntroduceMode(float delta);
	void updateMeteorMode(float delta);
	void updateSpaceCraftIntroduceMode(float delta);
	void updateSpaceCraftFightMode(float delta);
	void updateEnergyBarriersShowMode(float delta);
	void updateBossSpaceCraftIntroduceMode(float delta);
	void updateBossSpaceCraftFightMode(float delta);
	void updateBossSpaceCraftLeaveMode(float delta);

	void updateParallelEvents(float delta);

	void spawnMeteorites(float delta);
	void spawnHealthKits(float delta);
	void spawnEnemies(float delta);
	void spawnEnergyBarriers(float delta);
	void spawnEnemyBoss(float delta);

	void iterateLevel();

	virtual bool checkForCollisionAddiction(GameObject* obj1, GameObject* obj2);

	void setMaxNumberOfMeteorites(int number);
	void setMaxNumberOfHealthKits(int number);
	void setMaxNumberOfBarriers(int number);
	void setMaxNumberOfTeamEnemies(int number);

	void setMeteoritesZone(glm::vec2 zone);
	void setEnergyBarriersZone(glm::vec2 zone);
	void setHealthKitsZone(glm::vec2 zone);

	void setHealthKitsSpawnFreq(float freq);

	void addController(AIController* controller);

protected:
	SpacecraftObject* playerCraft = NULL;
	std::vector<AIController*> aiControllers;
	ISound* levelMusic = NULL;

	// meteorites data.
	int maxNumOfMeteorites = 0;
	int numOfCreatedMeteorites = 0;
	glm::vec2 meteoritesZone = glm::vec2(0, 300);		// min, max.

	// kits data.
	int maxNumOfHealthKits = 3;
	int numOfCreatedHealthKits = 0;
	float healthKitsFreq = 60.0f;
	float currentHealthKitsTime = 0.0f;
	glm::vec2 healthKitsZone = glm::vec2(-500, -1500);		// min, max.

	// enemies data.
	int maxNumOfBasicEnemies = 1;
	int numOfBasicEnemies = 0;

	int maxNumOfTeamEnemies = 0;
	int numOfTeamEnemies = 0;

	int maxNumOfBossEnemies = 1;
	int numOfBossEnemies = 0;
	float bossHealthThreshold;
	float bossHealthThresholdStep;
	bool introduceBegins = false;

	// energy barriers data.
	int maxNumOfBarriers = 0;
	int numOfCreatedBarriers = 0;
	glm::vec2 barriersZone = glm::vec2(0, 300);		// min, max.
};