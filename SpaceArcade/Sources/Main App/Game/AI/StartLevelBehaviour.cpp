#include "StartLevelBehaviour.h"

StartLevelBehaviour::StartLevelBehaviour()
{

}

StartLevelBehaviour::StartLevelBehaviour(GameLevel* level) : LevelBehaviour(level)
{

}

StartLevelBehaviour::~StartLevelBehaviour()
{
	clear();
}

void StartLevelBehaviour::init()
{

}

void StartLevelBehaviour::startBehaviour()
{
	for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
	{
		GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);

		// hide all ai controlled objects to start level with basic obstacles.
		if (spacecraft->getAIController())
			spacecraft->hideFromLevel(true);
	}
}

void StartLevelBehaviour::update(float delta)
{
	int numOfMeteorites = pLevel->getObjectsSizeByType(ObjectTypes::Meteorite);

	// show all ai controlled objects when all meteorites was destroyed or simply gone away.
	if (numOfMeteorites == 0)
	{
		for (int i = 0; i < pLevel->getObjectsSizeByType(ObjectTypes::SpaceCraft); ++i)
		{
			GameObject* spacecraft = pLevel->getObjectByTypeIndex(ObjectTypes::SpaceCraft, i);

			if (spacecraft->getAIController())
				spacecraft->hideFromLevel(false);
		}
	}
}

void StartLevelBehaviour::clear()
{

}