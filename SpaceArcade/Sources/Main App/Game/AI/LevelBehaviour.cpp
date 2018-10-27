#include "LevelBehaviour.h"

LevelBehaviour::LevelBehaviour()
{
	
}

LevelBehaviour::LevelBehaviour(GameLevel* level)
{
	pLevel = level;
	if (level)
		level->setBehaviour(this);
}

LevelBehaviour::~LevelBehaviour()
{
	clear();
}

void LevelBehaviour::init()
{

}

void LevelBehaviour::startBehaviour()
{
	
}

void LevelBehaviour::update(float delta)
{
	
}

void LevelBehaviour::setLevel(GameLevel* level)
{
	pLevel = level;
}

GameLevel* LevelBehaviour::getLevel()
{
	return pLevel;
}

void LevelBehaviour::clear()
{
	
}