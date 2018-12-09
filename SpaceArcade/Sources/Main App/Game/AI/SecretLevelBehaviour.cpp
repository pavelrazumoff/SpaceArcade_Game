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
*/

#include "SecretLevelBehaviour.h"
#include "BossShipAIController.h"
#include "TeamShipAIController.h"

SecretLevelBehaviour::SecretLevelBehaviour()
{
	init();
}

SecretLevelBehaviour::SecretLevelBehaviour(GameLevel* level, ResourceManager* resManager) :
	LevelBehaviour(level, resManager)
{
	init();
}

SecretLevelBehaviour::~SecretLevelBehaviour()
{
	clear();
}

void SecretLevelBehaviour::init()
{
	LevelBehaviour::init();

	levelMode = SecretLevelMode::SecretIntroducing;
	// block user input from beginning of the level.
	blockUserInput();
}

void SecretLevelBehaviour::startBehaviour()
{
	if (!playerCraft)
		return;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	playerCraft->Position = glm::vec2(screenDimensions.x / 2 - playerCraft->Size.x / 2, screenDimensions.y + 50);

	levelMusic = pLevel->playSound("BackgroundSound2", true);
}

void SecretLevelBehaviour::pauseBehaviour()
{
	if (levelMusic)
		levelMusic->setIsPaused(true);
}

void SecretLevelBehaviour::resumeBehaviour()
{
	if (levelMusic)
		levelMusic->setIsPaused(false);
}

void SecretLevelBehaviour::finishBehaviour()
{
	if (finishLevelCallback)
		finishLevelCallback();
}

void SecretLevelBehaviour::resetBehaviour()
{
	LevelBehaviour::resetBehaviour();
	playerCraft = NULL;
	pSpaceStation = NULL;

	for (int i = 0; i < aiControllers.size(); ++i)
		delete aiControllers[i];
	aiControllers.clear();

	if (levelMusic)
	{
		levelMusic->stop();
		levelMusic->drop();
		levelMusic = NULL;
	}

	levelData.playerSpeed = glm::vec2(200.0f, 100.0f);

	levelData.maxNumOfBlackHoles = 0;
	levelData.numOfCreatedBlackHoles = 0;
	levelData.blackHolesSpeed = glm::vec2(50, 100);
	levelData.blackHolesZone = glm::vec2(0, 300);

	levelMode = SecretLevelMode::SecretIntroducing;
	// block user input from beginning of the level.
	blockUserInput();

	if (updateLevelIterationCallback)
		updateLevelIterationCallback();
}

void SecretLevelBehaviour::restartLevelMusic()
{
	levelMusic = pLevel->playSound("BackgroundSound2", true);
}

void SecretLevelBehaviour::update(float delta)
{
	// handle only one mode in time.
	switch (levelMode)
	{
	case SecretLevelMode::SecretIntroducing:
		updateIntroduceMode(delta);
		break;
	case SecretLevelMode::BlackHolesFighting:
		updateBlackHolesMode(delta);
		break;
	case SecretLevelMode::SpaceStationIntroducing:
		updateSpaceStationIntroduceMode(delta);
		break;
	default:
		break;
	}

	// update complex ai controllers, such as team controllers.
	for (int i = 0; i < complexControllers.size(); ++i)
		complexControllers[i]->update(delta);

	updateParallelEvents(delta);
}

void SecretLevelBehaviour::updateIntroduceMode(float delta)
{
	// In this mode we have to raise playerCraft object on the screen from below.
	if (!playerCraft)
		return;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = pLevel->getRenderer()->getCurrentScreenDimensions() / pLevel->getRenderer()->getInitialScreenDimensions();

	// if playerCraft is not on specefied position, set it's velocity to force it to lift up.
	if (playerCraft->Position.y > (screenDimensions.y - pLevel->getPlayerRestrictionHeight() / 2))
	{
		playerCraft->Velocity.y = -80.0f * screenRatio.y;
		playerCraft->setUsePhysics(false);
	}
	else
	{
		// if playerCraft is on the position, reset it's velocity, enable physics,
		// unblock user input and change level mode to the next.
		playerCraft->Velocity.y = 0.0f;
		playerCraft->setUsePhysics(true);
		levelMode++;
		unblockUserInput();
	}
}

void SecretLevelBehaviour::updateBlackHolesMode(float delta)
{
	// In this mode we have to check, if all black holes was destroyed or gone behind sreen dimensions (not above the screen).
	int numOfBlackHoles = pLevel->getObjectsSizeByType(ObjectTypes::BlackHole);

	// show all ai controlled objects when all holes was destroyed or simply gone away.
	if (numOfBlackHoles == 0)
	{
		// change to the next level mode.
		levelMode++;
		blockUserInput();
	}
}

void SecretLevelBehaviour::updateSpaceStationIntroduceMode(float delta)
{
	if (!pSpaceStation)
		return;

	if (pSpaceStation->getStationState() == StationState::StationFree)
	{
		levelMode++;
		unblockUserInput();
		return;
	}

	if (abs(pSpaceStation->Position.y + pSpaceStation->Size.y / 2 - playerCraft->Position.y) < 300.0f)
	{
		pSpaceStation->Velocity = glm::vec2(0.0f, 0.0f);
		pSpaceStation->setSpaceCraft(playerCraft);
	}
}

void SecretLevelBehaviour::updateParallelEvents(float delta)
{
	// this function is called simultaneously to the other modes.
	// it updates all events that are parallel to the current mode.
	spawnBlackHoles(delta);

	if (levelMode == SecretLevelMode::SpaceStationIntroducing)
		spawnSpaceStation(delta);

	if (levelMode == SecretLevelMode::SecretEnd)
		spawnExitBlackHole(delta);

	// here we have to check if player craft is still alive.
	if (playerCraft && playerCraft->getHealth() <= 0.0f)
		finishBehaviour();
}

void SecretLevelBehaviour::spawnBlackHoles(float delta)
{
	// create some black holes by groups.
	int holesDiff = levelData.maxNumOfBlackHoles - levelData.numOfCreatedBlackHoles;
	if (holesDiff <= 0)
		return;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();
	glm::vec2 currentScreenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = (currentScreenDimensions / pLevel->getRenderer()->getInitialScreenDimensions());

	std::string textures[] = { "blackHole2", "blackHole3", "blackHole4" };

	glm::vec2 holeSize = glm::vec2(256, 256);

	float holeRatio = holeSize.x / holeSize.y;
	holeSize.x *= screenRatio.x;
	holeSize.y = holeSize.x / holeRatio;

	int denom = holesDiff;

	float yShift = screenRatio.y * abs(levelData.blackHolesZone.y - levelData.blackHolesZone.x) / denom;

	float currentShift = 0.0f;
	int currentPosY = -1;

	glm::vec2 holePoses[] = {
		glm::vec2(currentScreenDimensions.x / 2 - holeSize.x / 2, 0),
		glm::vec2(holeSize.x / 8, 1),
		glm::vec2(currentScreenDimensions.x - 9 * holeSize.x / 8, 2),
		glm::vec2(currentScreenDimensions.x / 2, 3),
		glm::vec2(currentScreenDimensions.x / 4, 3),
		glm::vec2(holeSize.x / 8, 4),
		glm::vec2(currentScreenDimensions.x - 1.5f * holeSize.x, 5),
		glm::vec2(currentScreenDimensions.x / 2 - holeSize.x / 2, 6),
		glm::vec2(holeSize.x / 8, 6),
		glm::vec2(currentScreenDimensions.x / 4, 7),
		glm::vec2(currentScreenDimensions.x - holeSize.x, 7),
		glm::vec2(currentScreenDimensions.x / 2 - holeSize.x / 2, 8),
		glm::vec2(3 * currentScreenDimensions.x / 4 - holeSize.x / 2, 8),
		glm::vec2(holeSize.x / 8, 9),
		glm::vec2(3 * currentScreenDimensions.x / 4 - holeSize.x / 2, 9),
		glm::vec2(currentScreenDimensions.x / 2 - holeSize.x, 10),
		glm::vec2(currentScreenDimensions.x - holeSize.x, 10),
		glm::vec2(3 * currentScreenDimensions.x / 4 - holeSize.x / 2, 11),
		glm::vec2(currentScreenDimensions.x / 2 - holeSize.x / 3, 12),
	};

	for (int i = 0; i < holesDiff; ++i)
	{
		int randIndex = rand() % 3;

		if (currentPosY != holePoses[i].y)
		{
			currentShift += -(rand() % (int)(yShift - yShift / 1.5f + 1) + yShift / 1.5f);
			currentPosY = holePoses[i].y;
		}

		BlackHoleObject* blackHole = new BlackHoleObject();
		blackHole->init(pLevel, glm::vec2(holePoses[i].x, currentShift),
			holeSize, pResourceManager->GetTexture(textures[randIndex]), glm::vec2(0.0f, levelData.blackHolesSpeed.y * screenRatio.y));
		blackHole->setDamage(0.15f);
		blackHole->setMaxHealth(1.0f);
		blackHole->setHealth(1.0f);
		blackHole->setScoreContribution(0);
		blackHole->InitialRotation = rand() % 360;
		blackHole->Rotation = 30.0f;
		blackHole->setBlackHoleType(BlackHoleType::Destroyer);
		blackHole->setImpulseFactor(200.0f);
	}

	levelData.numOfCreatedBlackHoles = levelData.maxNumOfBlackHoles;
}

void SecretLevelBehaviour::spawnSpaceStation(float delta)
{
	if (pSpaceStation)
		return;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();

	pSpaceStation = new SpaceStationObject();
	pSpaceStation->init(pLevel, glm::vec2(screenDimensions.x / 2 - 225, -550.0f), glm::vec2(550, 450), pResourceManager->GetTexture("spaceStation"), glm::vec2(0.0f, 100.0f));
	pSpaceStation->setCollisionCheck(false);
	pSpaceStation->setLandingPoint(glm::vec2(322, 355));
	pSpaceStation->setImpulseFactor(200.0f);
	pSpaceStation->setShowDialogueCallback(showStationDialogue);

	GameObject* roadster = new GameObject();
	roadster->init(pLevel, glm::vec2(0.0f, 0.0f), glm::vec2(80, 44), pResourceManager->GetTexture("roadster"), glm::vec2(0.0f, 0.0f), true);
	roadster->RelativePosition = glm::vec2(158, 96);
	roadster->InitialRotation = 45.0f;

	pSpaceStation->attachNewObject(roadster);
}

void SecretLevelBehaviour::spawnExitBlackHole(float delta)
{
	if (!pSpaceStation)
		return;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = screenDimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	glm::vec2 holeSize = glm::vec2(200, 200);

	float holeRatio = holeSize.x / holeSize.y;
	holeSize.x *= screenRatio.x;
	holeSize.y = holeSize.x / holeRatio;

	glm::vec2 holePos = glm::vec2(pSpaceStation->Position.x + 1.33f * pSpaceStation->Size.x, pSpaceStation->Position.y - pSpaceStation->Size.y / 2);

	if (holePos.x + holeSize.x > screenDimensions.x)
		holePos.x = screenDimensions.x - holeSize.x;

	if (holePos.y < 0.0f)
		holePos.y = 0.0f;

	BlackHoleObject* blackHole = new BlackHoleObject();
	blackHole->init(pLevel, holePos, holeSize, pResourceManager->GetTexture("blackHole1"), glm::vec2(0.0f, 40.0f));
	blackHole->setDamage(0.0f);
	blackHole->setMaxHealth(1.0f);
	blackHole->setHealth(1.0f);
	blackHole->setScoreContribution(0);
	blackHole->InitialRotation = rand() % 360;
	blackHole->Rotation = 60.0f;
	blackHole->setBlackHoleType(BlackHoleType::Teleportation);
	blackHole->setSelfDestroyTime(10.0f);
	blackHole->startSelfDestroying(true);

	levelMode = SecretLevelMode::SecretWaitForLeaving;
}

void SecretLevelBehaviour::iterateLevel()
{
	levelIteration++;

	if (updateLevelIterationCallback)
		updateLevelIterationCallback();
}

bool SecretLevelBehaviour::checkForCollisionAddiction(GameObject* obj1, GameObject* obj2)
{
	// immediately discard non-destroyable objects.
	switch (obj2->getObjectType())
	{
	case ObjectTypes::BlastWave:
	case ObjectTypes::ElectricShock:
	case ObjectTypes::ImprovementBox:
	case ObjectTypes::Coin:
	case ObjectTypes::EnergyBarrier:
	case ObjectTypes::None:
	case ObjectTypes::BlackHole:
	case ObjectTypes::SpaceStation:
		return false;
		break;
	default:
		break;
	}

	switch (obj1->getObjectType())
	{
	case ObjectTypes::Basic:
		return true;
		break;
	case ObjectTypes::LaserRay:
		if (obj2->getObjectType() != ObjectTypes::LaserRay &&
			obj2->getObjectType() != ObjectTypes::Rocket &&
			obj2->getObjectType() != ObjectTypes::EnergyShield)
			return true;
		else
			return false;
		break;
	case ObjectTypes::Meteorite:
	case ObjectTypes::Debris:
	case ObjectTypes::SpaceCraft:
		return true;
		break;
	case ObjectTypes::BossSpaceCraft:
		if (obj2->getObjectType() != ObjectTypes::EnergyShield)
			return true;
		else
			return false;
		break;
	case ObjectTypes::EnergyBarrier:
		if (obj2->getObjectType() != ObjectTypes::LaserRay)
			return true;
		else
			return false;
		break;
	case ObjectTypes::ElectricShock:
		if (obj2->getObjectType() != ObjectTypes::LaserRay &&
			obj2->getObjectType() != ObjectTypes::EnergyShield)
			return true;
		else
			return false;
		break;
	case ObjectTypes::BlastWave:
		if (obj2->getObjectType() != ObjectTypes::LaserRay &&
			obj2->getObjectType() != ObjectTypes::EnergyShield)
			return true;
		else
			return false;
		break;
	case ObjectTypes::ImprovementBox:
	case ObjectTypes::Coin:
		if (obj2->getObjectType() == ObjectTypes::SpaceCraft)
			return true;
		else
			return false;
		break;
	case ObjectTypes::Rocket:
		if (obj2->getObjectType() != ObjectTypes::LaserRay)
			return true;
		else
			return false;
		break;
	case ObjectTypes::EnergyShield:
		if (obj2->getObjectType() != ObjectTypes::BossSpaceCraft)
			return true;
		else
			return false;
		break;
	case ObjectTypes::BlackHole:
		return true;
		break;
	case ObjectTypes::SpaceStation:
		if (obj2->getObjectType() == ObjectTypes::SpaceCraft)
			return true;
		else
			return false;
		break;
	default:
		break;
	}

	return false;
}

void SecretLevelBehaviour::setPlayerObject(GameObject* obj)
{
	LevelBehaviour::setPlayerObject(obj);
	playerCraft = dynamic_cast<SpacecraftObject*>(obj);
}

void SecretLevelBehaviour::setMainBehaviour(LevelBehaviour* behaviour)
{
	pMainBehaviour = behaviour;
}

void SecretLevelBehaviour::setLevelData(SecretLevelData data)
{
	levelData = data;
}

SecretLevelData SecretLevelBehaviour::getLevelData()
{
	return levelData;
}

void SecretLevelBehaviour::addController(AIController* controller)
{
	aiControllers.push_back(controller);
}

void SecretLevelBehaviour::teleport(GameObject* object)
{
	// here we have to put selected object into another level.
	if (!pMainBehaviour)
		return;

	object->setParentObject(NULL);
	pSpaceStation->setSpaceCraft(NULL);

	levelMusic->stop();

	if (teleportPlayerCallback)
		teleportPlayerCallback(object, pMainBehaviour);

	levelMode = SecretLevelMode::SpaceStationIntroducing;
}

void SecretLevelBehaviour::finishStationDialogue()
{
	if(pSpaceStation)
		pSpaceStation->setStationState(StationState::SpaceCraftFree);
}

void SecretLevelBehaviour::clear()
{
	for (int i = 0; i < aiControllers.size(); ++i)
		delete aiControllers[i];

	aiControllers.clear();
}