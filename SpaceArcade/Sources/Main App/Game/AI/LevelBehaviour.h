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
	void blockUserInput();
	void unblockUserInput();
	virtual bool checkForCollisionAddiction(GameObject* obj1, GameObject* obj2) = 0;

	GameLevel* getLevel();
	bool isUserInputBlocked();

protected:
	GameLevel* pLevel = NULL;
	bool userInput = true;

	int levelMode = -1;
};