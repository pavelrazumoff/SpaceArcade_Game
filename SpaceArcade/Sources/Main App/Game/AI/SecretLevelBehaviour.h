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

	SecretLevelBehaviour.h
	This class is derived from LevelBehaviour. It presents secret level
	with alternating level modes one by one. This level is presented to
	give player an opportunity to get ion weapon from space station object
	to be able to defeat final boss enemy.
*/

#include "LevelBehaviour.h"
#include "../SpaceStationObject.h"

// enumeration of different level modes.
enum SecretLevelMode
{
	SecretIntroducing = 0,
	BlackHolesFighting,
	SpaceStationIntroducing,
	SecretEnd,
	SecretWaitForLeaving
};

// holds several level data to control level complexity.
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

protected:
	SpacecraftObject* playerCraft = NULL;
	SpaceStationObject* pSpaceStation = NULL;
	std::vector<AIController*> aiControllers;
	// pointer to the main behaviour (using in teleportation).
	LevelBehaviour* pMainBehaviour = NULL;

	// level's data.
	SecretLevelData levelData;
};