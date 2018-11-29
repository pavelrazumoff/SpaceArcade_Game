#pragma once
/*
	StartLevelBehaviour.h
	This class is derived from LevelBehaviour. It presents basic level
	with alternating level modes one by one. Base level objects here
	are playerCraft, enemyCraft and meteorites.
*/

#include "LevelBehaviour.h"
#include "../SpaceStationObject.h"

enum SecretLevelMode
{
	SecretIntroducing = 0,
	BlackHolesFighting,
	SpaceStationIntroducing,
	SecretEnd
};

struct SecretLevelData
{
	// player's data.
	glm::vec2 playerSpeed = glm::vec2(200.0f, 100.0f);

	// black holes data.
	int maxNumOfBlackHoles = 0;
	int numOfCreatedBlackHoles = 0;
	glm::vec2 blackHolesSpeed = glm::vec2(50, 100);		// min, max.
	glm::vec2 blackHolesZone = glm::vec2(0, 300);		// min, max.
};

class SecretLevelBehaviour : public LevelBehaviour
{
public:
	SecretLevelBehaviour();
	SecretLevelBehaviour(GameLevel* level, ResourceManager* resManager);
	virtual ~SecretLevelBehaviour();

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
	void updateBlackHolesMode(float delta);
	void updateSpaceStationIntroduceMode(float delta);

	void updateParallelEvents(float delta);

	void spawnBlackHoles(float delta);
	void spawnSpaceStation(float delta);
	void spawnExitBlackHole(float delta);

	void iterateLevel();

	virtual bool checkForCollisionAddiction(GameObject* obj1, GameObject* obj2);

	virtual void setPlayerObject(GameObject* obj);
	void setMainBehaviour(LevelBehaviour* behaviour);
	void setLevelData(SecretLevelData data);
	SecretLevelData getLevelData();

	void addController(AIController* controller);

	virtual void teleport(GameObject* object);
	void finishStationDialogue();

	void setFinishLevelCallback(void(*actionCallback)(void));
	void setIterateLevelCallback(void(*actionCallback)(void));
	void setTeleportPlayerCallback(void(*actionCallback)(GameObject*, LevelBehaviour*));

protected:
	SpacecraftObject* playerCraft = NULL;
	SpaceStationObject* pSpaceStation = NULL;
	std::vector<AIController*> aiControllers;
	LevelBehaviour* pMainBehaviour = NULL;

	// level's data.
	SecretLevelData levelData;

	void(*finishLevelCallback)(void) = NULL;
	void(*updateLevelIterationCallback)(void) = NULL;
	void(*teleportPlayerCallback)(GameObject*, LevelBehaviour*) = NULL;
};