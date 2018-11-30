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
	virtual void finishBehaviour();
	virtual void resetBehaviour();

	virtual void restartLevelMusic();

	virtual void clear();

	void setLevel(GameLevel* level);
	virtual void setPlayerObject(GameObject* obj);
	void blockUserInput();
	void blockUserAttack();
	void unblockUserInput();
	void unblockUserAttack();
	// returns true if both objects can collide with each other. This func has to be implemented in child classes.
	virtual bool checkForCollisionAddiction(GameObject* obj1, GameObject* obj2) = 0;

	GameLevel* getLevel();
	GameObject* getPlayerObject();
	bool isUserInputBlocked();
	bool isUserAttackBlocked();
	int getLevelIteration();

	// adds team controllers and others, that controls more than one pawn in one time.
	// these controllers are updating through level behaviour, not through pawn classes.
	void addComplexAIController(AIController* controller);
	// teleports selected object into another place, level and etc.
	virtual void teleport(GameObject* object);

	void setFinishLevelCallback(void(*actionCallback)(void));
	void setIterateLevelCallback(void(*actionCallback)(void));
	void setTeleportPlayerCallback(void(*actionCallback)(GameObject*, LevelBehaviour*));

protected:
	// pointer to the controlling level.
	GameLevel* pLevel = NULL;
	GameObject* playerObject = NULL;
	ResourceManager* pResourceManager = NULL;
	std::vector<AIController*> complexControllers;
	ISound* levelMusic = NULL;

	// false - all user input is blocked.
	bool userInput = true;
	// false - all user attack input is blocked, but he still can move.
	bool userAttack = true;

	// holds current sublevel.
	int levelMode = -1;
	// holds current level interation. Used to increase several level parameters
	// to increase level complexity.
	int levelIteration = 0;

	// these callback functions is used to send some data or commands into external code.
	// calls when player's craft lost.
	void(*finishLevelCallback)(void) = NULL;
	// calls after every level iteration.
	void(*updateLevelIterationCallback)(void) = NULL;
	// calls when player craft needs to be transferred to another behaviour (level).
	void(*teleportPlayerCallback)(GameObject*, LevelBehaviour*) = NULL;
};