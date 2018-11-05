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
	TeamCraftEnemyFighting
};

class StartLevelBehaviour : public LevelBehaviour
{
public:
	StartLevelBehaviour();
	StartLevelBehaviour(GameLevel* level);
	virtual ~StartLevelBehaviour();

	virtual void init();
	virtual void update(float delta);
	virtual void startBehaviour();

	virtual void clear();

	// only one of these functions is called in one time.
	// they are handling current level mode (sublevel) and when the time comes
	// change level mode to the next.
	void updateIntroduceMode(float delta);
	void updateMeteorMode(float delta);
	void updateSpaceCraftIntroduceMode(float delta);
	void updateSpaceCraftFightMode(float delta);
	void updateEnergyBarriersShowMode(float delta);

	virtual bool checkForCollisionAddiction(GameObject* obj1, GameObject* obj2);

protected:
	SpacecraftObject* playerCraft = NULL;
};