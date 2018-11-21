#include "StartLevelBehaviour.h"
#include "BossShipAIController.h"
#include "TeamShipAIController.h"

StartLevelBehaviour::StartLevelBehaviour()
{
	init();
}

StartLevelBehaviour::StartLevelBehaviour(GameLevel* level, ResourceManager* resManager) :
	LevelBehaviour(level, resManager)
{
	init();
}

StartLevelBehaviour::~StartLevelBehaviour()
{
	clear();
}

void StartLevelBehaviour::init()
{
	LevelBehaviour::init();

	levelMode = StartLevelMode::Introducing;
	// block user input from beginning of the level.
	blockUserInput();
}

void StartLevelBehaviour::startBehaviour()
{
	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = screenDimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	SpacecraftObject* pPlayerCraft = new SpacecraftObject();
	GameObject* pLaserRay = pPlayerCraft->getLaserRay();

	pPlayerCraft->setObjectType(ObjectTypes::SpaceCraft);
	pPlayerCraft->init(pLevel, glm::vec2(screenDimensions.x / 2 - 31, screenDimensions.y - 200), glm::vec2(62, 57), pResourceManager->GetTexture("spacecraft"), glm::vec2(0.0f, 0.0f));
	pPlayerCraft->VelocityScale = levelData.playerSpeed;
	pPlayerCraft->setExplosionSprite(pResourceManager->GetTexture("explosion"));
	pPlayerCraft->setHealthChangedCallback(healthBarChanged);
	pPlayerCraft->setEnergyChangedCallback(energyBarChanged);
	pPlayerCraft->setRocketIntegrityChangedCallback(rocketIntegrityChanged);
	pPlayerCraft->setNonPlayerObject(false);
	pPlayerCraft->setLaserSoundName("LaserSound");
	pPlayerCraft->setRocketSoundName("RocketSound");
	pPlayerCraft->setExplosionSoundName("ExplosionEffect");
	pPlayerCraft->setRocketStartVelocity(glm::vec2(0.0f, -450.0f * screenRatio.y));

	pLaserRay->init(pLevel, glm::vec2(0, 0), glm::vec2(13, 55), pResourceManager->GetTexture("laserRayBlue"), glm::vec2(0.0f, -400.0f * screenRatio.y), false);

	GameObject* pRocket = pPlayerCraft->getRocket();
	pRocket->init(pLevel, glm::vec2(0, 0), glm::vec2(16, 40), pResourceManager->GetTexture("rocket"), glm::vec2(0.0f, 0.0f), false);
	pRocket->setExplosionTime(1.0f);
	pRocket->setExplosionSprite(pResourceManager->GetTexture("explosion"));

	glm::vec2 holeSize = glm::vec2(200, 200);

	float holeRatio = holeSize.x / holeSize.y;
	holeSize.x *= screenRatio.x;
	holeSize.y = holeSize.x / holeRatio;

	BlackHoleObject* blackHole = new BlackHoleObject();
	blackHole->init(pLevel, glm::vec2(0, 0), holeSize, pResourceManager->GetTexture("blackHole1"), glm::vec2(0.0f, 40.0f));
	blackHole->setDamage(0.0f);
	blackHole->setMaxHealth(1.0f);
	blackHole->setHealth(1.0f);
	blackHole->setScoreContribution(0);
	blackHole->InitialRotation = rand() % 360;
	blackHole->Rotation = 60.0f;
	blackHole->setBlackHoleType(BlackHoleType::Teleportation);
	blackHole->setSelfDestroyTime(5.0f);

	pPlayerCraft->setBlackHole(blackHole);

	GameObject* ionWeapon = new GameObject();
	ionWeapon->init(pLevel, glm::vec2(0, 0), glm::vec2(46, 47), pResourceManager->GetTexture("ionWeapon"), glm::vec2(0.0f, 0.0f), true);

	pPlayerCraft->setIonWeapon(ionWeapon);

	pPlayerCraft->setRocketRelativePosition(glm::vec2(42, 12), 0);
	pPlayerCraft->setRocketRelativePosition(glm::vec2(3, 12), 1);
	pPlayerCraft->setRocketRelativePosition(glm::vec2(21, -5), 2);

	pPlayerCraft->Position = glm::vec2(screenDimensions.x / 2 - pPlayerCraft->Size.x / 2, screenDimensions.y + 50);
	pPlayerCraft->setHealth(pPlayerCraft->getHealth());
	pPlayerCraft->setRocketDetail(0);

	pPlayerCraft->setCoins(4000);

	setPlayerObject(pPlayerCraft);
	levelMusic = pLevel->playSound("BackgroundSound", true);
}

void StartLevelBehaviour::pauseBehaviour()
{
	if(levelMusic)
		levelMusic->setIsPaused(true);
}

void StartLevelBehaviour::resumeBehaviour()
{
	if (levelMusic)
		levelMusic->setIsPaused(false);
}

void StartLevelBehaviour::finishBehaviour()
{
	if (finishLevelCallback)
		finishLevelCallback();
}

void StartLevelBehaviour::resetBehaviour()
{
	LevelBehaviour::resetBehaviour();
	playerCraft = NULL;

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
	levelData.meteoritesSpeed = glm::vec2(50, 150);
	levelData.debrisSpeed = glm::vec2(50, 150);
	levelData.basicEnemySpeed = 100.0f;
	levelData.bossEnemySpeed = 100.0f;
	levelData.barriersSpeed = 100.0f;

	levelData.numOfCreatedMeteorites = 0;
	levelData.numOfCreatedHealthKits = 0;
	levelData.numOfCreatedDebris = 0;
	levelData.currentHealthKitsTime = 0.0f;

	levelData.numOfBasicEnemies = 0;
	levelData.numOfTeamEnemies = 0;
	levelData.numOfBossEnemies = 0;
	levelData.introduceBegins = false;
	levelData.numOfCreatedBarriers = 0;

	levelData.timeWithoutShield = 0.0f;

	levelData.bossHealth = 1000.0f;
	levelData.bossEnergy = 200.0f;

	levelMode = StartLevelMode::Introducing;
	// block user input from beginning of the level.
	blockUserInput();

	if (updateLevelIterationCallback)
		updateLevelIterationCallback();
}

void StartLevelBehaviour::update(float delta)
{
	// handle only one mode in time.
	switch (levelMode)
	{
	case StartLevelMode::Introducing:
		updateIntroduceMode(delta);
		break;
	case StartLevelMode::MeteorFighting:
		updateMeteorMode(delta);
		break;
	case StartLevelMode::SpaceCraftEnemyIntroducing:
		updateSpaceCraftIntroduceMode(delta);
		break;
	case StartLevelMode::SpaceCraftEnemyFighting:
		updateSpaceCraftFightMode(delta);
		break;
	case StartLevelMode::EnergyBarriersShowing:
		updateEnergyBarriersShowMode(delta);
		break;
	case StartLevelMode::TeamCraftEnemyIntroducing:
		updateSpaceCraftIntroduceMode(delta);
		break;
	case StartLevelMode::TeamCraftEnemyFighting:
		updateSpaceCraftFightMode(delta);
		break;
	case StartLevelMode::BossSpaceCraftIntroducing:
		updateBossSpaceCraftIntroduceMode(delta);
		break;
	case StartLevelMode::BossSpaceCraftFighting:
		updateBossSpaceCraftFightMode(delta);
		break;
	case StartLevelMode::BossSpaceCraftLeaving:
		updateBossSpaceCraftLeaveMode(delta);
		break;
	case StartLevelMode::DebrisFighting:
		updateDebrisMode(delta);
		break;
	case StartLevelMode::End:
		levelMode = StartLevelMode::MeteorFighting;
		iterateLevel();
		break;
	default:
		break;
	}

	// update complex ai controllers, such as team controllers.
	for (int i = 0; i < complexControllers.size(); ++i)
		complexControllers[i]->update(delta);

	updateParallelEvents(delta);
}

void StartLevelBehaviour::updateIntroduceMode(float delta)
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

void StartLevelBehaviour::updateMeteorMode(float delta)
{
	// In this mode we have to check, if all meteorites was destroyed or gone behind sreen dimensions (not above the screen).
	int numOfMeteorites = pLevel->getObjectsSizeByType(ObjectTypes::Meteorite);

	// show all ai controlled objects when all meteorites was destroyed or simply gone away.
	if (numOfMeteorites == 0)
	{
		// change to the next level mode.
		levelMode++;
		blockUserInput();
	}
}

void StartLevelBehaviour::updateSpaceCraftIntroduceMode(float delta)
{
	// In this mode we simply introducing all ai controlled spacecrafts on the screen.
	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = pLevel->getRenderer()->getCurrentScreenDimensions() / pLevel->getRenderer()->getInitialScreenDimensions();

	spawnEnemies(delta);
	bool updateLevel = true;

	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
	{
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);

		if (spacecraft->getAIController())
		{
			if (spacecraft->Position.y < (screenDimensions.y / 3 - spacecraft->Size.y / 2))
			{
				spacecraft->Velocity.y = -100.0f * screenRatio.y;
				spacecraft->setUsePhysics(false);
				updateLevel = false;
			}
			else
			{
				spacecraft->Velocity.y = 0.0f;
				spacecraft->setUsePhysics(true);
				spacecraft->getAIController()->unblockAI();
				unblockUserInput();
			}
		}
	}

	if(updateLevel)
		levelMode++;
}

void StartLevelBehaviour::updateSpaceCraftFightMode(float delta)
{
	// In this mode a battle between player's spacecraft and enemies happens.
	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
	{
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);
		if (spacecraft->getAIController())
			return;
	}

	levelMode++;

	switch (levelMode)
	{
	case StartLevelMode::EnergyBarriersShowing:
		spawnEnergyBarriers(delta);
		return;
	case StartLevelMode::BossSpaceCraftIntroducing:
		blockUserInput();
		return;
	default:
		break;
	}
}

void StartLevelBehaviour::updateEnergyBarriersShowMode(float delta)
{
	int numOfBarriers = pLevel->getObjectsSizeByType(ObjectTypes::EnergyBarrier);

	// show all ai controlled objects when all meteorites was destroyed or simply gone away.
	if (numOfBarriers == 0)
	{
		// change to the next level mode.
		levelMode++;
		blockUserInput();
	}
}

void StartLevelBehaviour::updateBossSpaceCraftIntroduceMode(float delta)
{
	// In this mode we simply introducing all ai controlled spacecrafts on the screen.
	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = pLevel->getRenderer()->getCurrentScreenDimensions() / pLevel->getRenderer()->getInitialScreenDimensions();

	spawnEnemyBoss(delta);
	bool updateLevel = true;

	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::BossSpaceCraft); ++i)
	{
		BossSpacecraftObject* spacecraft = (BossSpacecraftObject*)pLevel->getObjectByTypeIndex(ObjectTypes::BossSpaceCraft, i);

		if (spacecraft->getAIController())
		{
			// reset boss starting introduce position in case screen size was changed.
			if (!levelData.introduceBegins)
			{
				spacecraft->Position.y = -spacecraft->Size.y - 10.0f;
				levelData.introduceBegins = true;
			}

			if (spacecraft->Position.y < (screenDimensions.y / 3 - spacecraft->Size.y / 2))
			{
				spacecraft->Velocity.y = -50.0f * screenRatio.y;
				spacecraft->setUsePhysics(false);
				updateLevel = false;
				spacecraft->hideFromLevel(false);
			}
			else
			{
				spacecraft->Velocity.y = 0.0f;
				spacecraft->setUsePhysics(true);
				spacecraft->enableShield(true);
				spacecraft->getAIController()->unblockAI();
				unblockUserInput();
				levelData.introduceBegins = false;
			}
		}
	}

	if (updateLevel)
		levelMode++;
}

void StartLevelBehaviour::updateBossSpaceCraftFightMode(float delta)
{
	// In this mode a battle between player's spacecraft and enemies happens.
	bool isBossAlive = false;
	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::BossSpaceCraft); ++i)
	{
		BossSpacecraftObject* spacecraft = (BossSpacecraftObject*)pLevel->getObjectByTypeIndex(ObjectTypes::BossSpaceCraft, i);
		if (spacecraft->getAIController())
		{
			if (spacecraft->getHealth() > levelData.bossHealthThreshold)
			{
				if (!spacecraft->isShieldEnabled())
				{
					if (levelData.timeWithoutShield >= levelData.maxTimeWithoutShield)
					{
						spacecraft->enableShield(true);
						levelData.timeWithoutShield = 0.0f;
					}
					else
						levelData.timeWithoutShield += delta;
				}
				return;
			}
			else
			{
				if (spacecraft->getHealth() > 0.0f)
				{
					isBossAlive = true;
					if (!spacecraft->isShieldEnabled())
						spacecraft->enableShield(true);
				}

				levelData.bossHealthThreshold -= levelData.bossHealthThresholdStep;
				if (levelData.bossHealthThreshold < 0.0f)
					levelData.bossHealthThreshold = 0.0f;
				spacecraft->getAIController()->BlockAI();
			}
		}
	}

	if (isBossAlive)
	{
		levelMode++;
		blockUserAttack();
	}
	else
	{
		levelData.numOfBossEnemies = 0;
		// 11.
		if (levelIteration < 1)
			levelMode = StartLevelMode::End;		// skip boss leaving mode.
		else
			levelMode = StartLevelMode::DebrisFighting;
	}
}

void StartLevelBehaviour::updateBossSpaceCraftLeaveMode(float delta)
{
	// In this mode we simply introducing all ai controlled spacecrafts on the screen.
	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = pLevel->getRenderer()->getCurrentScreenDimensions() / pLevel->getRenderer()->getInitialScreenDimensions();

	bool updateLevel = true;

	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::BossSpaceCraft); ++i)
	{
		BossSpacecraftObject* spacecraft = (BossSpacecraftObject*)pLevel->getObjectByTypeIndex(ObjectTypes::BossSpaceCraft, i);

		if (spacecraft->getAIController() && spacecraft->getHealth() > 0.0f)
		{
			if (spacecraft->Position.y + spacecraft->Size.y > -10.0f)
			{
				spacecraft->Velocity.y = 150.0f * screenRatio.y;
				spacecraft->setUsePhysics(false);
				updateLevel = false;
			}
			else
			{
				spacecraft->Velocity.y = 0.0f;
				unblockUserAttack();
				spacecraft->hideFromLevel(true);
				levelData.timeWithoutShield = 0.0f;
			}
		}
	}

	if (updateLevel)
		levelMode = StartLevelMode::End;
}

void StartLevelBehaviour::updateDebrisMode(float delta)
{
	// In this mode we have to check, if all meteorites was destroyed or gone behind sreen dimensions (not above the screen).
	int numOfDebris = pLevel->getObjectsSizeByType(ObjectTypes::Debris);

	// show all ai controlled objects when all meteorites was destroyed or simply gone away.
	if (numOfDebris == 0)
	{
		// change to the next level mode.
		levelMode++;
	}
}

void StartLevelBehaviour::updateParallelEvents(float delta)
{
	// this function is called simultaneously to the other modes.
	// it updates all events that are parallel to the current mode.
	spawnMeteorites(delta);
	spawnHealthKits(delta);

	if(levelMode == StartLevelMode::DebrisFighting)
		spawnDebris(delta);

	// here we have to check if player craft is still alive.
	if (playerCraft)
	{
		if (playerCraft->getHealth() <= 0.0f)
		{
			finishBehaviour();
			return;
		}
	}
}

void StartLevelBehaviour::spawnMeteorites(float delta)
{
	// create some meteorites by groups.
	glm::vec2 screenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();

	int meteoriteDiff = levelData.maxNumOfMeteorites - levelData.numOfCreatedMeteorites;
	if (meteoriteDiff <= 0)
		return;

	glm::vec2 detailSizes[] = {
		glm::vec2(15, 37),
		glm::vec2(24, 28),
		glm::vec2(24, 36)
	};

	ImprovementStruct rocketKit;
	rocketKit.rocketDetail = 40;
	rocketKit.useRocketDetail = true;

	int numOfMeteorites = 5;
	if (meteoriteDiff < numOfMeteorites)
		numOfMeteorites = meteoriteDiff;

	for (int i = 0; i < numOfMeteorites; ++i)
	{
		GameObject* asteroid = new GameObject();
		asteroid->setMaxHealth(20.0f);
		asteroid->setHealth(20.0f);
		asteroid->setDamage(10.0f);
		asteroid->setExplosionTime(1.0f);
		asteroid->setExplosionSprite(pResourceManager->GetTexture("explosion"));
		asteroid->setUsePhysics(true);
		asteroid->setObjectType(ObjectTypes::Meteorite);
		asteroid->setExplosionSoundName("ExplosionEffect2");
		//asteroid->hideFromLevel(true);

		asteroid->init(pLevel, glm::vec2(rand() % ((int)screenDimensions.x - 100 + 1) + 50,
			rand() % (int)(levelData.meteoritesZone.x - levelData.meteoritesZone.y + 1) + levelData.meteoritesZone.y), glm::vec2(50, 50),
			pResourceManager->GetTexture("asteroid"), glm::vec2(rand() % 15,
				rand() % (int)(levelData.meteoritesSpeed.y - levelData.meteoritesSpeed.x + 1) + levelData.meteoritesSpeed.x));

		asteroid->InitialRotation = rand() % 360;
		asteroid->Rotation = 10.0f;

		asteroid->resize();

		if (rand() % levelData.meteorRandomSeed == 1)
		{
			ImprovementBoxObject* rocketDetail = new ImprovementBoxObject();

			rocketDetail->init(pLevel, glm::vec2(0.0f, 0.0f), detailSizes[i % 3], pResourceManager->GetTexture("rocketDetail1_" + std::to_string(i % 3)),
				glm::vec2(rand() % (20 + 1) - 20, rand() % (170 - 60 + 1) + 60));

			rocketDetail->InitialRotation = rand() % 360;
			rocketDetail->Rotation = 15.0f;
			rocketDetail->setImprovement(rocketKit);

			GameObject* smoke = new GameObject();
			smoke->setUsePhysics(false);
			smoke->setObjectType(ObjectTypes::Basic);
			smoke->setSelfDestroyTime(1.5f);
			smoke->setUseAnimation(true);
			smoke->setCollisionCheck(false);
			smoke->setAnimationDuration(1.5f);

			smoke->init(pLevel, glm::vec2(0.0f, 0.0f), glm::vec2(64, 64), pResourceManager->GetTexture("smoke"), glm::vec2(0.0f, 0.0f));

			rocketDetail->setSmokeObject(smoke);

			rocketDetail->resize();
			asteroid->addPostDeathObject(rocketDetail);
		}
	}

	levelData.numOfCreatedMeteorites += numOfMeteorites;
}

void StartLevelBehaviour::spawnHealthKits(float delta)
{
	// Spawn Health Kits every healthKitsFreq time.
	levelData.currentHealthKitsTime += delta;

	if (levelData.currentHealthKitsTime >= levelData.healthKitsFreq)
	{
		levelData.numOfCreatedHealthKits = 0;
		levelData.currentHealthKitsTime = 0.0f;
	}

	glm::vec2 screenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();
	int kitsDiff = levelData.maxNumOfHealthKits - levelData.numOfCreatedHealthKits;

	if (kitsDiff <= 0)
		return;

	for (int i = 0; i < kitsDiff; ++i)
	{
		ImprovementBoxObject* box = new ImprovementBoxObject();

		float posY = levelData.healthKitsZone.x - i * (rand() % (int)(abs(levelData.healthKitsZone.y - levelData.healthKitsZone.x) / kitsDiff) + 500);
		box->init(pLevel, glm::vec2(rand() % ((int)screenDimensions.x - 300 + 1) + 100, posY),
			glm::vec2(45, 45), pResourceManager->GetTexture("healthKit"), glm::vec2(0.0f, 70.0f));

		box->InitialRotation = rand() % 360;
		box->Rotation = 15.0f;
		box->setScoreContribution(0);

		ImprovementStruct healthKit;
		healthKit.health = 30.0f;
		healthKit.useHealth = true;

		box->setImprovement(healthKit);

		GameObject* smoke = new GameObject();
		smoke->setUsePhysics(false);
		smoke->setObjectType(ObjectTypes::Basic);
		smoke->setSelfDestroyTime(1.5f);
		smoke->setUseAnimation(true);
		smoke->setCollisionCheck(false);
		smoke->setAnimationDuration(1.5f);
		smoke->color = glm::vec4(0.0f, 1.0f, 0.0f, 0.8f);

		smoke->init(pLevel, glm::vec2(0.0f, 0.0f), glm::vec2(128, 128), pResourceManager->GetTexture("smoke"), glm::vec2(0.0f, 0.0f));

		box->setSmokeObject(smoke);
		box->resize();
	}

	levelData.numOfCreatedHealthKits = levelData.maxNumOfHealthKits;
}

void StartLevelBehaviour::spawnEnemies(float delta)
{
	int craftNum = 0;
	TeamShipAIController* teamController = NULL;

	switch (levelMode)
	{
	case StartLevelMode::SpaceCraftEnemyIntroducing:
		craftNum = levelData.maxNumOfBasicEnemies - levelData.numOfBasicEnemies;
		levelData.numOfBasicEnemies = levelData.maxNumOfBasicEnemies;
		break;
	case StartLevelMode::TeamCraftEnemyIntroducing:
		craftNum = levelData.maxNumOfTeamEnemies - levelData.numOfTeamEnemies;
		levelData.numOfTeamEnemies = levelData.maxNumOfTeamEnemies;
		break;
	default:
		break;
	}

	if (craftNum <= 0)
		return;

	if (levelMode == StartLevelMode::TeamCraftEnemyIntroducing)
	{
		teamController = new TeamShipAIController();
		addController(teamController);
		addComplexAIController(teamController);
		teamController->setTargetEnemy(playerCraft);
	}

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = screenDimensions / pLevel->getRenderer()->getInitialScreenDimensions();
	float zoneWidth = screenDimensions.x / craftNum;

	for (int i = 0; i < craftNum; ++i)
	{
		BasicShipAIController* spacecraftAI = new BasicShipAIController();
		addController(spacecraftAI);
		spacecraftAI->setSourcePosition(glm::vec2(0.5f, 0.5f));
		spacecraftAI->setControlledArea(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		SpacecraftObject* enemySpaceCraft = new SpacecraftObject();
		enemySpaceCraft->init(pLevel, glm::vec2(0.0f, 0.0f), glm::vec2(85, 92), pResourceManager->GetTexture("spacecraftEnemy"), glm::vec2(0.0f, 0.0f));
		enemySpaceCraft->InitialRotation = 180.0f;
		enemySpaceCraft->VelocityScale = glm::vec2(levelData.basicEnemySpeed, 100.0f);
		enemySpaceCraft->setExplosionSprite(pResourceManager->GetTexture("explosion"));
		enemySpaceCraft->setAIController(spacecraftAI);
		enemySpaceCraft->setControlVelocityByRotation(true);
		enemySpaceCraft->setMaxHealth(200.0f);
		enemySpaceCraft->setHealth(200.0f);
		enemySpaceCraft->setLaserSoundName("LaserEnemySound");
		enemySpaceCraft->setExplosionSoundName("ExplosionEffect");
		enemySpaceCraft->setScoreContribution(50);

		GameObject* pLaserRay = enemySpaceCraft->getLaserRay();
		pLaserRay->setObjectType(ObjectTypes::LaserRay);
		pLaserRay->init(pLevel, glm::vec2(0, 0), glm::vec2(13, 55), pResourceManager->GetTexture("laserRayRed"), glm::vec2(0.0f, -400.0f * screenRatio.y), false);

		if (levelMode == StartLevelMode::TeamCraftEnemyIntroducing)
			teamController->addController(spacecraftAI);

		// resize it just before setting actual position.
		//enemySpaceCraft->resize();

		// show ai controlled spacecraft and position it a little above the screen.
		enemySpaceCraft->Position = glm::vec2(zoneWidth / 2 + i * zoneWidth - 42, -100.0f);
		if (levelMode == StartLevelMode::SpaceCraftEnemyIntroducing)
			enemySpaceCraft->getAIController()->setTargetEnemy(playerCraft);

		// also block its ai for a little time to be able to properly introduce this object on scene.
		enemySpaceCraft->getAIController()->BlockAI();
	}
}

void StartLevelBehaviour::spawnEnemyBoss(float delta)
{
	if (levelData.numOfBossEnemies > 0)
		return;

	levelData.bossHealth += 50.0f;
	levelData.bossEnergy += 20.0f;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = screenDimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	BossShipAIController* bossAI = new BossShipAIController();
	addController(bossAI);
	bossAI->setSourcePosition(glm::vec2(0.5f, 0.5f));
	bossAI->setControlledArea(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	BossSpacecraftObject* bossSpaceCraft = new BossSpacecraftObject();
	bossSpaceCraft->init(pLevel, glm::vec2(0.0f, 0.0f), glm::vec2(218, 134), pResourceManager->GetTexture("spacecraftBoss"), glm::vec2(0.0f, 0.0f));
	bossSpaceCraft->InitialRotation = 180.0f;
	bossSpaceCraft->VelocityScale = glm::vec2(levelData.bossEnemySpeed, 100.0f);
	bossSpaceCraft->setExplosionSprite(pResourceManager->GetTexture("explosion"));
	bossSpaceCraft->setAIController(bossAI);
	bossSpaceCraft->setControlVelocityByRotation(true);
	bossSpaceCraft->setMaxHealth(levelData.bossHealth);
	bossSpaceCraft->setHealth(levelData.bossHealth);
	bossSpaceCraft->setMaxEnergy(levelData.bossEnergy);
	bossSpaceCraft->setLaserSoundName("LaserEnemySound");
	bossSpaceCraft->setExplosionSoundName("ExplosionEffect");
	bossSpaceCraft->setScoreContribution(300);

	GameObject* pLaserRay = bossSpaceCraft->getLaserRay();
	pLaserRay->setObjectType(ObjectTypes::LaserRay);
	pLaserRay->init(pLevel, glm::vec2(0, 0), glm::vec2(13, 55), pResourceManager->GetTexture("laserRayRed"), glm::vec2(0.0f, -400.0f * screenRatio.y), false);

	glm::vec2 laserPoints[] = {
		glm::vec2(50, 54),
		glm::vec2(160, 54),
		glm::vec2(15, 80),
		glm::vec2(200, 80),
		glm::vec2(106, 60),
	};

	for (int i = 0; i < 5; ++i)
		bossSpaceCraft->addLaserStartPoint(laserPoints[i]);
	bossSpaceCraft->setIndexOfPreferredLaserPoint(4);

	GameObject* energyShield = new GameObject();

	energyShield->init(pLevel, glm::vec2(0, 0), glm::vec2(289, 249), pResourceManager->GetTexture("energyShield"), glm::vec2(0.0f, 0.0f));
	energyShield->RelativePosition = glm::vec2(-35, -47);
	energyShield->setUseAnimation(true);
	energyShield->setAnimationDuration(0.5f);
	energyShield->setUseBackAndForthAnimation(true);
	energyShield->setObjectType(ObjectTypes::EnergyShield);
	energyShield->setScoreContribution(20);

	bossSpaceCraft->setEnergyShield(energyShield);

	// resize it just before setting actual position.
	//bossSpaceCraft->resize();

	// show ai controlled spacecraft and position it a little above the screen.
	bossSpaceCraft->getAIController()->setTargetEnemy(playerCraft);
	bossSpaceCraft->Position = glm::vec2(screenDimensions.x / 2 - 109, -200.0f);
	// also block its ai for a little time to be able to properly introduce this object on scene.
	bossSpaceCraft->getAIController()->BlockAI();

	levelData.bossHealthThreshold = (2.0f * bossSpaceCraft->getHealth()) / 3.0f;
	levelData.bossHealthThresholdStep = bossSpaceCraft->getHealth() / 3.0f;
	levelData.numOfBossEnemies++;
}

void StartLevelBehaviour::spawnEnergyBarriers(float delta)
{
	int barriersDiff = levelData.maxNumOfBarriers - levelData.numOfCreatedBarriers;

	if (barriersDiff <= 0)
		return;

	glm::vec2 initialScreenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();
	glm::vec2 currentScreenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = (currentScreenDimensions / initialScreenDimensions);

	int denom = (7 * barriersDiff / 16);
	if (denom == 0)
		denom = 7;

	float yShift = screenRatio.y * abs(levelData.barriersZone.y - levelData.barriersZone.x) / denom;

	float currentShift = 0.0f;
	int currentPosY = -1;
	int posesDecrement = 0;
	
	glm::vec2 barrierSize = glm::vec2(131, 35);
	glm::vec2 generatorSize = glm::vec2(33, 32);
	glm::vec2 blastWaveSize = glm::vec2(128, 128);

	float objRatio = barrierSize.x / barrierSize.y;
	barrierSize.x *= screenRatio.x;
	barrierSize.y = barrierSize.x / objRatio;

	objRatio = generatorSize.x / generatorSize.y;
	generatorSize.x *= screenRatio.x;
	generatorSize.y = generatorSize.x / objRatio;

	objRatio = blastWaveSize.x / blastWaveSize.y;
	blastWaveSize.x *= screenRatio.x;
	blastWaveSize.y = blastWaveSize.x / objRatio;

	float barrierWidth = barrierSize.x + generatorSize.x;

	glm::vec2 barrierPoses[] = {
		glm::vec2(generatorSize.x, 0),
		glm::vec2(currentScreenDimensions.x - barrierWidth, 0),
		glm::vec2(currentScreenDimensions.x / 2 - barrierWidth / 2, 1),
		glm::vec2(currentScreenDimensions.x / 4 - barrierWidth / 2, 2),
		glm::vec2(3 * currentScreenDimensions.x / 4 - barrierWidth / 2, 2),
		glm::vec2(currentScreenDimensions.x / 6 - barrierWidth / 2, 3),
		glm::vec2(currentScreenDimensions.x / 2 - barrierWidth / 2, 3),
		glm::vec2(5 * currentScreenDimensions.x / 6 - barrierWidth / 2, 3),
		glm::vec2(2 * currentScreenDimensions.x / 5 - barrierWidth / 2, 4),
		glm::vec2(3 * currentScreenDimensions.x / 5 - barrierWidth / 2, 4),
		glm::vec2(currentScreenDimensions.x / 5 - barrierWidth / 2, 5),
		glm::vec2(3 * currentScreenDimensions.x / 4 - barrierWidth / 2, 5),
		glm::vec2(generatorSize.x, 6),
		glm::vec2(2 * currentScreenDimensions.x / 5 - barrierWidth / 2, 6),
		glm::vec2(3 * currentScreenDimensions.x / 5 - barrierWidth / 2, 6),
		glm::vec2(currentScreenDimensions.x - barrierWidth, 6),
	};

	for (int i = 0; i < barriersDiff; ++i)
	{
		if (i > 0 && i / 16 > 0 && i % 16 == 0)
			posesDecrement += 16;

		if (currentPosY != barrierPoses[i - posesDecrement].y)
		{
			currentShift += -(rand() % (int)(yShift - yShift / 1.5f + 1) + yShift / 1.5f);
			currentPosY = barrierPoses[i - posesDecrement].y;
		}

		EnergyBarrierObject* barrier = new EnergyBarrierObject();

		barrier->init(pLevel, glm::vec2(barrierPoses[i - posesDecrement].x, currentShift), barrierSize,
			pResourceManager->GetTexture("energyBarrier"), glm::vec2(0.0f, levelData.barriersSpeed * screenRatio.y));
		barrier->setAnimationDuration(0.5f);
		barrier->setGeneratorSoundName("GeneratorEffect");
		barrier->setScoreContribution(0);

		GameObject* generators[2];
		std::string texes[] = { "leftGenerator", "rightGenerator" };
		for (int j = 0; j < 2; ++j)
		{
			generators[j] = new GameObject();
			generators[j]->setMaxHealth(80.0f);
			generators[j]->setHealth(80.0f);
			generators[j]->setDamage(10.0f);
			generators[j]->setExplosionTime(1.0f);
			generators[j]->setExplosionSprite(pResourceManager->GetTexture("explosion"));
			generators[j]->setUsePhysics(true);
			generators[j]->setObjectType(ObjectTypes::Basic);
			generators[j]->setExplosionSoundName("ExplosionEffect2");
			generators[j]->setScoreContribution(15);

			generators[j]->init(pLevel, glm::vec2(0.0f, 0.0f), generatorSize, pResourceManager->GetTexture(texes[j]), glm::vec2(0.0f, 0.0f));
		}

		barrier->setGenerators(generators[0], generators[1]);

		GameObject* electricShock = new GameObject();
		electricShock->setDamage(0.1f);
		electricShock->setMaxHealth(1000.0f);
		electricShock->setHealth(1000.0f);
		electricShock->setUsePhysics(false);
		electricShock->setObjectType(ObjectTypes::ElectricShock);
		electricShock->init(NULL, glm::vec2(0.0f, 0.0f), glm::vec2(96, 84), pResourceManager->GetTexture("electricShock"), glm::vec2(0.0f, 0.0f));
		electricShock->RelativePosition = glm::vec2(0.0f, 0.0f);
		electricShock->setAnimationDuration(0.5f);
		electricShock->setUseAnimation(true);
		electricShock->setSelfDestroyTime(1.5f);

		barrier->setElectricShock(electricShock);

		BlastWaveObject* blastWave = new BlastWaveObject();
		blastWave->init(pLevel, glm::vec2(0.0f, 0.0f), blastWaveSize, pResourceManager->GetTexture("blastWave"), glm::vec2(0.0f, 0.0f));
		blastWave->setAnimationDuration(0.5f);
		blastWave->setSelfDestroyTime(0.5f);
		blastWave->setExplosionSoundName("ElectricExplosionEffect");

		barrier->setBlastWave(blastWave);
	}

	levelData.numOfCreatedBarriers = levelData.maxNumOfBarriers;
}

void StartLevelBehaviour::spawnDebris(float delta)
{
	// create some debris by groups.
	glm::vec2 screenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();

	int debrisDiff = levelData.maxNumOfDebris - levelData.numOfCreatedDebris;
	if (debrisDiff <= 0)
		return;

	glm::vec2 debrisSizes[] = {
		glm::vec2(22, 24),
		glm::vec2(30, 53),
		glm::vec2(40, 50),
		glm::vec2(34, 32),
		glm::vec2(39, 30),
		glm::vec2(45, 45)
	};

	std::string textures[] = { "debris1_0", "debris1_1", "debris1_2",
		"debris1_3", "debris1_4", "debris1_5" };

	int numOfDebris = 5;
	if (debrisDiff < numOfDebris)
		numOfDebris = debrisDiff;

	for (int i = 0; i < numOfDebris; ++i)
	{
		int randIndex = rand() % 5;

		GameObject* wreck = new GameObject();
		wreck->setMaxHealth(20.0f);
		wreck->setHealth(20.0f);
		wreck->setDamage(10.0f);
		wreck->setExplosionTime(1.0f);
		wreck->setExplosionSprite(pResourceManager->GetTexture("explosion"));
		wreck->setUsePhysics(true);
		wreck->setObjectType(ObjectTypes::Debris);
		wreck->setExplosionSoundName("ExplosionEffect2");

		wreck->init(pLevel, glm::vec2(rand() % ((int)screenDimensions.x - 100 + 1) + 50,
			rand() % (int)(levelData.debrisZone.x - levelData.debrisZone.y + 1) + levelData.debrisZone.y), debrisSizes[randIndex],
			pResourceManager->GetTexture(textures[randIndex]), glm::vec2(rand() % 15,
				rand() % (int)(levelData.debrisSpeed.y - levelData.debrisSpeed.x + 1) + levelData.debrisSpeed.x));

		wreck->InitialRotation = rand() % 360;
		wreck->Rotation = 10.0f;

		wreck->resize();
	}

	if (!levelData.secretWreckCreated)
	{
		ImprovementBoxObject* secretWreck = new ImprovementBoxObject();

		secretWreck->init(pLevel, glm::vec2(rand() % ((int)screenDimensions.x - 100 + 1) + 50,
			rand() % (int)(levelData.debrisZone.x - levelData.debrisZone.y + 1) + levelData.debrisZone.y), debrisSizes[5],
			pResourceManager->GetTexture(textures[5]), glm::vec2(rand() % 15,
				rand() % (int)(levelData.debrisSpeed.y - levelData.debrisSpeed.x + 1) + levelData.debrisSpeed.x));

		secretWreck->InitialRotation = rand() % 360;
		secretWreck->Rotation = 15.0f;
		secretWreck->setScoreContribution(0);

		ImprovementStruct blackHoleKit;
		blackHoleKit.blackHole = true;
		blackHoleKit.useBlackHole = true;

		secretWreck->setImprovement(blackHoleKit);

		GameObject* smoke = new GameObject();
		smoke->setUsePhysics(false);
		smoke->setObjectType(ObjectTypes::Basic);
		smoke->setSelfDestroyTime(1.5f);
		smoke->setUseAnimation(true);
		smoke->setCollisionCheck(false);
		smoke->setAnimationDuration(1.5f);
		smoke->color = glm::vec4(0.0f, 0.0f, 1.0f, 0.8f);

		smoke->init(pLevel, glm::vec2(0.0f, 0.0f), glm::vec2(128, 128), pResourceManager->GetTexture("smoke"), glm::vec2(0.0f, 0.0f));

		secretWreck->setSmokeObject(smoke);
		secretWreck->resize();
		levelData.secretWreckCreated = true;
	}

	levelData.numOfCreatedDebris += numOfDebris;
}

void StartLevelBehaviour::iterateLevel()
{
	levelIteration++;

	// player.
	if (levelIteration % 4 == 0)
		levelData.playerSpeed += glm::vec2(20.0f, 10.0f);

	// meteorites.
	levelData.numOfCreatedMeteorites = 0;
	levelData.maxNumOfMeteorites += 10;
	levelData.meteoritesZone.y *= 1.2f;
	levelData.meteoritesSpeed.x += 10.0f;
	levelData.meteoritesSpeed.y += 15.0f;

	if (levelIteration % 2 == 0)
		levelData.meteorRandomSeed++;

	// health kits.
	if (levelIteration % 4 == 0)
		levelData.maxNumOfHealthKits++;

	// energy barriers.
	levelData.numOfCreatedBarriers = 0;
	levelData.maxNumOfBarriers += 4;
	levelData.barriersZone.y *= 1.1f;
	levelData.barriersSpeed += 10.0f;

	// basic enemies.
	levelData.numOfBasicEnemies = 0;
	if(levelData.basicEnemySpeed < 350.0f)
		levelData.basicEnemySpeed += 10.0f;

	// team enemies.
	levelData.numOfTeamEnemies = 0;
	if (levelIteration <= 6 && levelIteration % 2 == 0)
		levelData.maxNumOfTeamEnemies++;

	// boss enemy.
	if (levelData.bossEnemySpeed < 350.0f)
		levelData.bossEnemySpeed += 10.0f;
	if (levelIteration <= 14 && levelIteration % 2 == 0 && levelData.maxTimeWithoutShield > 5)
		levelData.maxTimeWithoutShield -= 1.0f;

	// debris.
	// 11.
	if (levelIteration > 1)
	{
		levelData.numOfCreatedDebris = 0;
		levelData.maxNumOfDebris += 10;
		levelData.debrisZone.y *= 1.2f;
		levelData.debrisSpeed.x += 10.0f;
		levelData.debrisSpeed.y += 15.0f;

		if (levelIteration % 2 == 0)
			levelData.debrisRandomSeed++;
	}

	levelData.secretWreckCreated = false;

	if (updateLevelIterationCallback)
		updateLevelIterationCallback();
}

bool StartLevelBehaviour::checkForCollisionAddiction(GameObject* obj1, GameObject* obj2)
{
	// immediately discard non-destroyable objects.
	switch (obj2->getObjectType())
	{
	case ObjectTypes::BlastWave:
	case ObjectTypes::ElectricShock:
	case ObjectTypes::ImprovementBox:
	case ObjectTypes::EnergyBarrier:
	case ObjectTypes::None:
	case ObjectTypes::BlackHole:
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
	default:
		break;
	}

	return false;
}

void StartLevelBehaviour::setPlayerObject(GameObject* obj)
{
	LevelBehaviour::setPlayerObject(obj);
	playerCraft = dynamic_cast<SpacecraftObject*>(obj);
}

void StartLevelBehaviour::setSecretBehaviour(LevelBehaviour* behaviour)
{
	pSecretBehaviour = behaviour;
}

void StartLevelBehaviour::setLevelData(StartLevelData data)
{
	levelData = data;
}

StartLevelData StartLevelBehaviour::getLevelData()
{
	return levelData;
}

void StartLevelBehaviour::addController(AIController* controller)
{
	aiControllers.push_back(controller);
}

void StartLevelBehaviour::teleport(GameObject* object)
{
	// here we have to put selected object into another level.
	if (!pSecretBehaviour)
		return;
	if (teleportPlayerCallback)
		teleportPlayerCallback(object, pSecretBehaviour);
}

void StartLevelBehaviour::setFinishLevelCallback(void(*actionCallback)(void))
{
	finishLevelCallback = actionCallback;
}

void StartLevelBehaviour::setIterateLevelCallback(void(*actionCallback)(void))
{
	updateLevelIterationCallback = actionCallback;
}

void StartLevelBehaviour::setTeleportPlayerCallback(void(*actionCallback)(GameObject*, LevelBehaviour*))
{
	teleportPlayerCallback = actionCallback;
}

void StartLevelBehaviour::clear()
{
	for (int i = 0; i < aiControllers.size(); ++i)
		delete aiControllers[i];

	aiControllers.clear();
}