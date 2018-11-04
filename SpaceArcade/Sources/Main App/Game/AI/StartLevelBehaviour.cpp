#include "StartLevelBehaviour.h"
#include "AIController.h"

StartLevelBehaviour::StartLevelBehaviour()
{
	init();
}

StartLevelBehaviour::StartLevelBehaviour(GameLevel* level) : LevelBehaviour(level)
{
	init();
}

StartLevelBehaviour::~StartLevelBehaviour()
{
	clear();
}

void StartLevelBehaviour::init()
{
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
			playerCraft = dynamic_cast<SpacecraftObject*>(spacecraft);
			playerCraft->Position = glm::vec2(screenDimensions.x / 2 - playerCraft->Size.x / 2, screenDimensions.y + 50);
		}
	}

	pLevel->playSound("BackgroundSound", true);
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
	default:
		break;
	}
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
		levelMode = StartLevelMode::MeteorFighting;
		unblockUserInput();
	}
}

void StartLevelBehaviour::updateMeteorMode(float delta)
{
	// In this mode we have to check, if all meteorites was destroyed or gone behind sreen dimensions (not above the screen).
	int numOfMeteorites = pLevel->getObjectsSizeByType(ObjectTypes::Meteorite);
	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();

	// show all ai controlled objects when all meteorites was destroyed or simply gone away.
	if (numOfMeteorites == 0)
	{
		for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
		{
			GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);

			if (spacecraft->getAIController())
			{
				// show ai controlled spacecraft and position it a little above the screen.
				spacecraft->hideFromLevel(false);
				spacecraft->Position = glm::vec2(screenDimensions.x / 2 - spacecraft->Size.x / 2, -100);
				// also block it's ai for a little time to be able to properly introduce this object on scene.
				spacecraft->getAIController()->BlockAI();
			}
		}

		// change to the next level mode.
		levelMode = StartLevelMode::SpaceCraftEnemyIntroducing;
		blockUserInput();
	}
}

void StartLevelBehaviour::updateSpaceCraftIntroduceMode(float delta)
{
	// In this mode we simply introducing all ai controlled spacecrafts on the screen.
	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = pLevel->getRenderer()->getCurrentScreenDimensions() / pLevel->getRenderer()->getInitialScreenDimensions();

	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
	{
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);

		if (spacecraft->getAIController())
		{
			if (spacecraft->Position.y < (screenDimensions.y / 3 - spacecraft->Size.y / 2))
			{
				spacecraft->Velocity.y = -100.0f * screenRatio.y;
				spacecraft->setUsePhysics(false);
			}
			else
			{
				spacecraft->Velocity.y = 0.0f;
				spacecraft->setUsePhysics(true);
				spacecraft->getAIController()->unblockAI();
				levelMode = StartLevelMode::SpaceCraftEnemyFighting;
				unblockUserInput();
			}
		}
	}
}

void StartLevelBehaviour::updateSpaceCraftFightMode(float delta)
{
	// In this mode a battle between player's spacecraft and enemies happens.
	bool modePassed = true;
	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
	{
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);
		if (spacecraft->getAIController())
		{
			modePassed = false;
			break;
		}
	}

	if (modePassed)
	{
		levelMode = StartLevelMode::EnergyBarriersShowing;
		for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::EnergyBarrier); ++i)
			pLevel->getObjectByTypeIndex(ObjectTypes::EnergyBarrier, i)->hideFromLevel(false);
	}
}

void StartLevelBehaviour::updateEnergyBarriersShowMode(float delta)
{
	
}

bool StartLevelBehaviour::checkForCollisionAddiction(GameObject* obj1, GameObject* obj2)
{
	// immediately discard non-destroyable objects.
	switch (obj2->getObjectType())
	{
	case ObjectTypes::EnergyBarrier:
	case ObjectTypes::BlastWave:
	case ObjectTypes::ElectricShock:
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
	{
		if (obj2->getObjectType() != ObjectTypes::LaserRay)
			return true;
		else
			return false;
	}
		break;
	case ObjectTypes::Meteorite:
	case ObjectTypes::SpaceCraft:
		return true;
		break;
	case ObjectTypes::EnergyBarrier:
		if (obj2->getObjectType() != ObjectTypes::LaserRay)
			return true;
		else
			return false;
		break;
	case ObjectTypes::ElectricShock:
		if (obj2->getObjectType() != ObjectTypes::LaserRay)
			return true;
		else
			return false;
		break;
	case ObjectTypes::BlastWave:
		if (obj2->getObjectType() != ObjectTypes::LaserRay)
			return true;
		else
			return false;
		break;
	default:
		break;
	}

	return false;
}

void StartLevelBehaviour::clear()
{

}