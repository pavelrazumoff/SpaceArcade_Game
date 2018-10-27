#pragma once
#include "../GameLevel.h"

class LevelBehaviour
{
public:
	LevelBehaviour();
	LevelBehaviour(GameLevel* level);
	virtual ~LevelBehaviour();

	virtual void init();
	virtual void update(float delta);
	virtual void startBehaviour();

	virtual void clear();

	void setLevel(GameLevel* level);

	GameLevel* getLevel();

protected:
	GameLevel* pLevel = NULL;
};