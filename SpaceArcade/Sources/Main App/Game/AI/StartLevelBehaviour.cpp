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
	blockUserInput();
}

void StartLevelBehaviour::startBehaviour()
{
	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
	{
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);
		glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();

		// hide all ai controlled objects to start level with basic obstacles.
		if (spacecraft->getAIController())
			spacecraft->hideFromLevel(true);
		if (!spacecraft->isNonPlayerObject())
		{
			playerCraft = dynamic_cast<SpacecraftObject*>(spacecraft);
			playerCraft->Position = glm::vec2(screenDimensions.x / 2 - playerCraft->Size.x / 2, screenDimensions.y + 50);
		}
	}
}

void StartLevelBehaviour::update(float delta)
{
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
	default:
		break;
	}
}

void StartLevelBehaviour::updateIntroduceMode(float delta)
{
	if (!playerCraft)
		return;

	glm::vec2 screenDimensions = pLevel->getRenderer()->getCurrentScreenDimensions();
	glm::vec2 screenRatio = pLevel->getRenderer()->getCurrentScreenDimensions() / pLevel->getRenderer()->getInitialScreenDimensions();

	if (playerCraft->Position.y > (screenDimensions.y - pLevel->getPlayerRestrictionHeight() / 2))
	{
		playerCraft->Velocity.y = -80.0f * screenRatio.y;
		playerCraft->setUsePhysics(false);
	}
	else
	{
		playerCraft->Velocity.y = 0.0f;
		playerCraft->setUsePhysics(true);
		levelMode = StartLevelMode::MeteorFighting;
		unblockUserInput();
	}
}

void StartLevelBehaviour::updateMeteorMode(float delta)
{
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
				spacecraft->hideFromLevel(false);
				spacecraft->Position = glm::vec2(screenDimensions.x / 2 - spacecraft->Size.x / 2, -100);
				spacecraft->getAIController()->BlockAI();
			}
		}

		levelMode = StartLevelMode::SpaceCraftEnemyIntroducing;
		blockUserInput();
	}
}

void StartLevelBehaviour::updateSpaceCraftIntroduceMode(float delta)
{
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
	
}

bool StartLevelBehaviour::checkForCollisionAddiction(GameObject* obj1, GameObject* obj2)
{
	switch (obj1->getObjectType())
	{
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
	default:
		break;
	}

	return false;
}

void StartLevelBehaviour::clear()
{

}