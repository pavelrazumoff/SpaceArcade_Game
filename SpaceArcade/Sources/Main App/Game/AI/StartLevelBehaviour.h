#pragma once
/*
	* Space Arcade Game
	* Copyright (C) 2018 Paul Razumov.
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	* Source:
	*  https://github.com/pavelrazumoff/SpaceArcade_Game

	StartLevelBehaviour.h
	This class is derived from LevelBehaviour. It presents basic level
	with alternating level modes one by one.
*/

#include "LevelBehaviour.h"

// enumeration of different level modes.
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
	DebrisFighting,
	FinalBossSpaceCraftIntroducing,
	FinalBossSpaceCraftFighting,
	FinalBossSpaceCraftLeaving,
	End
};

// holds several level data to control level complexity.
struct StartLevelData
{
	// player's data.
	glm::vec2 playerSpeed = glm::vec2(200.0f, 100.0f);
	int starmanLevel = 5;

	// meteorites data.
	int maxNumOfMeteorites = 0;
	int numOfCreatedMeteorites = 0;
	int meteorRandomSeed = 8;
	glm::vec2 meteoritesSpeed = glm::vec2(50, 150);		// min, max.
	glm::vec2 meteoritesZone = glm::vec2(0, 300);		// min, max.

	// debris data.
	int maxNumOfDebris = 0;
	int numOfCreatedDebris = 0;
	int debrisRandomSeed = 8;
	glm::vec2 debrisSpeed = glm::vec2(50, 150);		// min, max.
	glm::vec2 debrisZone = glm::vec2(0, 300);		// min, max.
	bool secretWreckCreated = false;

	// kits data.
	int maxNumOfHealthKits = 3;
	int numOfCreatedHealthKits = 0;
	float healthKitsFreq = 60.0f;
	float currentHealthKitsTime = 0.0f;
	glm::vec2 healthKitsZone = glm::vec2(-500, -1500);		// min, max.

	// coins data.
	int coinRandomSeed = 4;

	// enemies data.
	int maxNumOfBasicEnemies = 1;
	int numOfBasicEnemies = 0;
	float basicEnemySpeed = 100.0f;

	int maxNumOfTeamEnemies = 0;
	int numOfTeamEnemies = 0;

	// boss data.
	int maxNumOfBossEnemies = 1;
	int numOfBossEnemies = 0;
	float bossHealthThreshold;
	float bossHealthThresholdStep;
	float bossHealth = 1000.0f;
	float bossEnergy = 200.0f;
	float bossEnemySpeed = 100.0f;
	bool introduceBegins = false;
	float timeWithoutShield = 0.0f;
	float maxTimeWithoutShield = 10.0f;

	// final boss data.
	float finalBossHealth = 2000.0f;
	float finalBossEnergy = 300.0f;
	float finalBossEnemySpeed = 150.0f;
	bool finalBossWasDefeated = false;
	bool battleStarted = false;

	// energy barriers data.
	int maxNumOfBarriers = 0;
	int numOfCreatedBarriers = 0;
	float barriersSpeed = 100.0f;
	glm::vec2 barriersZone = glm::vec2(0, 300);		// min, max.
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
	virtual void finishBehaviour();
	virtual void resetBehaviour();

	virtual void restartLevelMusic();

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
	void updateDebrisMode(float delta);
	void updateFinalBossSpaceCraftIntroduceMode(float delta);
	void updateFinalBossSpaceCraftFightMode(float delta);
	void updateFinalBossSpaceCraftLeaveMode(float delta);

	void updateParallelEvents(float delta);

	void spawnMeteorites(float delta);
	void spawnHealthKits(float delta);
	void spawnEnemies(float delta);
	void spawnEnergyBarriers(float delta);
	void spawnEnemyBoss(float delta);
	void spawnFinalBoss(float delta);
	void spawnDebris(float delta);
	void showStarman(float delta);

	void spawnCoinWithObject(GameObject* obj, int numOfCoins);

	void iterateLevel();

	virtual bool checkForCollisionAddiction(GameObject* obj1, GameObject* obj2);

	virtual void setPlayerObject(GameObject* obj);
	void setSecretBehaviour(LevelBehaviour* behaviour);
	void setLevelData(StartLevelData data);
	StartLevelData getLevelData();

	void addController(AIController* controller);

	virtual void teleport(GameObject* object);

	void setShowEnemyBarCallback(void(*actionCallback)(bool));

protected:
	SpacecraftObject* playerCraft = NULL;
	std::vector<AIController*> aiControllers;
	LevelBehaviour* pSecretBehaviour = NULL;
	BossSpacecraftObject* pFinalBoss = NULL;
	GameObject* flyingStarman = NULL;

	bool bossEnemyBarWasShown = false;

	// level's data.
	StartLevelData levelData;

	// these callback functions is used to send some data or commands into external code.
	// calls when enemy boss is appearing to visualize it's health status.
	void(*showEnemyBarCallback)(bool) = NULL;
};