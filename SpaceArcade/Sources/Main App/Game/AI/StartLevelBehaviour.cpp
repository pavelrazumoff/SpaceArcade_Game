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
	// get all ai controlled spacecrafts and hide them.
	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
	{
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);
		glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();

		// hide all ai controlled objects to start level with basic obstacles.
		if (spacecraft->getAIController())
			spacecraft->hideFromLevel(true);

		// if this spacecraft is player controlled, save it and position it a little below the screen.
		if (!spacecraft->isNonPlayerObject())
		{
			playerObject = spacecraft;
			playerCraft = dynamic_cast<SpacecraftObject*>(spacecraft);
			playerCraft->Position = glm::vec2(screenDimensions.x / 2 - playerCraft->Size.x / 2, screenDimensions.y + 50);
		}
	}

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
			if (!introduceBegins)
			{
				spacecraft->Position.y = -spacecraft->Size.y - 10.0f;
				introduceBegins = true;
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
				introduceBegins = false;
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
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::BossSpaceCraft, i);
		if (spacecraft->getAIController())
		{
			if (spacecraft->getHealth() > bossHealthThreshold)
				return;
			else
			{
				if (spacecraft->getHealth() > 0.0f)
					isBossAlive = true;

				bossHealthThreshold -= bossHealthThresholdStep;
				if (bossHealthThreshold < 0.0f)
					bossHealthThreshold = 0.0f;
				spacecraft->getAIController()->BlockAI();
			}
		}
	}

	if (isBossAlive)
	{
		levelMode++;
		blockUserInput();
	}
	else
	{
		numOfBossEnemies = 0;
		levelMode += 2;		// skip boss leaving mode.
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
				unblockUserInput();
				spacecraft->enableShield(false);
				spacecraft->hideFromLevel(true);
			}
		}
	}

	if (updateLevel)
		levelMode++;
}

void StartLevelBehaviour::updateParallelEvents(float delta)
{
	// this function is called simultaneously to the other modes.
	// it updates all events that are parallel to the current mode.
	spawnMeteorites(delta);
	spawnHealthKits(delta);
}

void StartLevelBehaviour::spawnMeteorites(float delta)
{
	// create some meteorites by groups.
	glm::vec2 screenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();

	int meteoriteDiff = maxNumOfMeteorites - numOfCreatedMeteorites;
	if (meteoriteDiff <= 0)
		return;
	
	int randomIndices[10];

	for (int i = 0; i < 10; ++i)
		randomIndices[i] = rand() % (30);

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
			rand() % (int)(meteoritesZone.x + meteoritesZone.y + 1) - meteoritesZone.y), glm::vec2(46, 47),
			pResourceManager->GetTexture("asteroid"), glm::vec2(rand() % 15, rand() % (150 - 50 + 1) + 50));

		asteroid->InitialRotation = rand() % 360;
		asteroid->Rotation = 10.0f;

		asteroid->resize();

		for (int j = 0; j < 10; ++j)
			if (i == randomIndices[j])
			{
				ImprovementBoxObject* rocketDetail = new ImprovementBoxObject();

				rocketDetail->init(pLevel, glm::vec2(0.0f, 0.0f), detailSizes[i % 3], pResourceManager->GetTexture("rocketDetail1_" + std::to_string(i % 3)),
					glm::vec2(rand() % (20 + 1) - 20, rand() % (170 - 60 + 1) + 60));

				rocketDetail->InitialRotation = rand() % 360;
				rocketDetail->Rotation = 15.0f;
				rocketDetail->setImprovement(rocketKit);

				rocketDetail->resize();
				asteroid->addPostDeathObject(rocketDetail);
				break;
			}
	}

	numOfCreatedMeteorites += numOfMeteorites;
}

void StartLevelBehaviour::spawnHealthKits(float delta)
{
	// Spawn Health Kits every healthKitsFreq time.
	currentHealthKitsTime += delta;

	if (currentHealthKitsTime >= healthKitsFreq)
	{
		numOfCreatedHealthKits = 0;
		currentHealthKitsTime = 0.0f;
	}

	glm::vec2 screenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();
	int kitsDiff = maxNumOfHealthKits - numOfCreatedHealthKits;

	if (kitsDiff <= 0)
		return;

	for (int i = 0; i < kitsDiff; ++i)
	{
		ImprovementBoxObject* box = new ImprovementBoxObject();

		float posY = healthKitsZone.x - i * (rand() % (int)(abs(healthKitsZone.y - healthKitsZone.x) / kitsDiff) + 500);
		box->init(pLevel, glm::vec2(rand() % ((int)screenDimensions.x - 300 + 1) + 100, posY),
			glm::vec2(45, 45), pResourceManager->GetTexture("healthKit"), glm::vec2(0.0f, 70.0f));

		box->InitialRotation = rand() % 360;
		box->Rotation = 15.0f;
		box->setScoreContribution(0);

		ImprovementStruct healthKit;
		healthKit.health = 30.0f;
		healthKit.useHealth = true;

		box->setImprovement(healthKit);

		box->resize();
	}

	numOfCreatedHealthKits = maxNumOfHealthKits;
}

void StartLevelBehaviour::spawnEnemies(float delta)
{
	int craftNum = 0;
	TeamShipAIController* teamController = NULL;

	switch (levelMode)
	{
	case StartLevelMode::SpaceCraftEnemyIntroducing:
		craftNum = maxNumOfBasicEnemies - numOfBasicEnemies;
		numOfBasicEnemies = maxNumOfBasicEnemies;
		break;
	case StartLevelMode::TeamCraftEnemyIntroducing:
		craftNum = maxNumOfTeamEnemies - numOfTeamEnemies;
		numOfTeamEnemies = maxNumOfTeamEnemies;
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
		enemySpaceCraft->VelocityScale = glm::vec2(200.0f, 100.0f);
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
	if (numOfBossEnemies > 0)
		return;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = screenDimensions / pLevel->getRenderer()->getInitialScreenDimensions();

	BossShipAIController* bossAI = new BossShipAIController();
	addController(bossAI);
	bossAI->setSourcePosition(glm::vec2(0.5f, 0.5f));
	bossAI->setControlledArea(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	BossSpacecraftObject* bossSpaceCraft = new BossSpacecraftObject();
	bossSpaceCraft->init(pLevel, glm::vec2(0.0f, 0.0f), glm::vec2(218, 134), pResourceManager->GetTexture("spacecraftBoss"), glm::vec2(0.0f, 0.0f));
	bossSpaceCraft->InitialRotation = 180.0f;
	bossSpaceCraft->VelocityScale = glm::vec2(100.0f, 100.0f);
	bossSpaceCraft->setExplosionSprite(pResourceManager->GetTexture("explosion"));
	bossSpaceCraft->setAIController(bossAI);
	bossSpaceCraft->setControlVelocityByRotation(true);
	bossSpaceCraft->setMaxHealth(1000.0f);
	bossSpaceCraft->setHealth(1000.0f);
	bossSpaceCraft->setMaxEnergy(200.0f);
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

	bossHealthThreshold = (2.0f * bossSpaceCraft->getHealth()) / 3.0f;
	bossHealthThresholdStep = bossSpaceCraft->getHealth() / 3.0f;
	numOfBossEnemies++;
}

void StartLevelBehaviour::spawnEnergyBarriers(float delta)
{
	glm::vec2 screenDimensions = pLevel->getRenderer()->getInitialScreenDimensions();
	int barriersDiff = maxNumOfBarriers - numOfCreatedBarriers;

	if (barriersDiff <= 0)
		return;

	glm::vec2 currentScreenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = (currentScreenDimensions / screenDimensions);

	int denom = (7 * barriersDiff / 16);
	if (denom == 0)
		denom = 7;

	float yShift = screenRatio.y * abs(barriersZone.y - barriersZone.x) / denom;

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
			pResourceManager->GetTexture("energyBarrier"), glm::vec2(0.0f, 120.0f));
		barrier->setAnimationDuration(0.5f);
		barrier->setGeneratorSoundName("GeneratorEffect");
		barrier->setScoreContribution(0);

		GameObject* generators[2];
		std::string texes[] = { "leftGenerator", "rightGenerator" };
		for (int j = 0; j < 2; ++j)
		{
			generators[j] = new GameObject();
			generators[j]->setMaxHealth(40.0f);
			generators[j]->setHealth(40.0f);
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

	numOfCreatedBarriers = maxNumOfBarriers;
}

void StartLevelBehaviour::iterateLevel()
{
	levelIteration++;

	// meteorites.
	numOfCreatedMeteorites = 0;
	maxNumOfMeteorites += 10;
	meteoritesZone.y *= 1.2f;

	// health kits.
	if (levelIteration % 4 == 0)
		maxNumOfHealthKits++;

	// energy barriers.
	numOfCreatedBarriers = 0;
	maxNumOfBarriers += 4;
	barriersZone.y *= 1.1f;

	// basic enemies.
	numOfBasicEnemies = 0;

	// team enemies.
	numOfTeamEnemies = 0;
	if (levelIteration % 2 == 0)
		maxNumOfTeamEnemies++;
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
	default:
		break;
	}

	return false;
}

void StartLevelBehaviour::setMaxNumberOfMeteorites(int number)
{
	maxNumOfMeteorites = number;
}

void StartLevelBehaviour::setMaxNumberOfHealthKits(int number)
{
	maxNumOfHealthKits = number;
}

void StartLevelBehaviour::setMaxNumberOfBarriers(int number)
{
	maxNumOfBarriers = number;
}

void StartLevelBehaviour::setMaxNumberOfTeamEnemies(int number)
{
	maxNumOfTeamEnemies = number;
}

void StartLevelBehaviour::setMeteoritesZone(glm::vec2 zone)
{
	meteoritesZone = zone;
}

void StartLevelBehaviour::setEnergyBarriersZone(glm::vec2 zone)
{
	barriersZone = zone;
}

void StartLevelBehaviour::setHealthKitsZone(glm::vec2 zone)
{
	healthKitsZone = zone;
}

void StartLevelBehaviour::setHealthKitsSpawnFreq(float freq)
{
	healthKitsFreq = freq;
}

void StartLevelBehaviour::addController(AIController* controller)
{
	aiControllers.push_back(controller);
}

void StartLevelBehaviour::clear()
{
	for (int i = 0; i < aiControllers.size(); ++i)
		delete aiControllers[i];

	aiControllers.clear();
}