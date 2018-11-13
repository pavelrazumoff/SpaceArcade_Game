#pragma once
/*
	LevelBehaviour.h
	This class manages level behaviour. In fact it let us to use one level
	for different level's possible types by pushing all handling stuff into
	derived classes from this. It helps us to divide level into sublevels by
	controlling levelMode variable.
*/

#include "../GameLevel.h"
#include "../../../Miscellaneous/ResourceManager.h"

class LevelBehaviour
{
public:
	LevelBehaviour();
	LevelBehaviour(GameLevel* level, ResourceManager* resManager);
	virtual ~LevelBehaviour();

	virtual void init();
	virtual void update(float delta);
	// this func calls only once in startLevel function after all level's initializations have been completed.
	virtual void startBehaviour();
	virtual void pauseBehaviour();
	virtual void resumeBehaviour();

	virtual void clear();

	void setLevel(GameLevel* level);
	void blockUserInput();
	void unblockUserInput();
	// returns true if both objects can collide with each other. This func has to be implemented in child classes.
	virtual bool checkForCollisionAddiction(GameObject* obj1, GameObject* obj2) = 0;

	GameLevel* getLevel();
	GameObject* getPlayerObject();
	bool isUserInputBlocked();

	void addComplexAIController(AIController* controller);

protected:
	// pointer to the controlling level.
	GameLevel* pLevel = NULL;
	GameObject* playerObject = NULL;
	ResourceManager* pResourceManager = NULL;
	std::vector<AIController*> complexControllers;

	// false - all user input is blocked.
	bool userInput = true;

	// holds current sublevel.
	int levelMode = -1;
	int levelIteration = 0;
};