#pragma once
/*
	* Space Arcade Game
	* Copyright (C) 2018 Paul Razumov.
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	* Source:
	*  https://github.com/pavelrazumoff/SpaceArcade_Game

	GameLevel.h
	This class holds all level data, that handles for collision checking, updating,
	drawing and other things.
*/

#include "BossSpacecraftObject.h"
#include "EnergyBarrierObject.h"
#include "ImprovementBoxObject.h"
#include "../../Miscellaneous/QuadTree.h"

class LevelBehaviour;

enum LevelStatus
{
	NotStarted = 0,
	Playing,
	Paused,
	Overed
};

class GameLevel
{
public:
	// Constructor
	GameLevel() { }
	~GameLevel();

	void									init(Texture2D* cubemap, Shader cubemapShader, SpriteRenderer* renderer);
	void									resize();
	void									clear();

	void									draw();
	void									update(float delta);
	void									updatePaused(float delta);
	void									updateInstances();
	void									startLevel();			// calls once after all initializations and before updating.
	void									pauseLevel();
	void									resumeLevel();
	void									finishLevel();			// calls when game was overed.
	void									resetLevel();

	void									handleInput(GLFWwindow *window, float delta);
	void									processKey(int key, int action, bool* key_pressed);

	void									doCollisions();

	void									addNewObject(GameObject* obj);
	void									addExternalObject(GameObject* obj);
	void									removeObject(GameObject* obj);
	void									removeExternalObject(GameObject* obj);

	void									addSound(std::string soundName, std::string soundPath);
	ISound*									playSound(std::string soundName, bool loop);

	void									setScoreChangedCallback(void(*actionCallback)(int));

	void									setBehaviour(LevelBehaviour* behaviour);
	void									setScreenIndents(glm::vec4 indents);
	void									setPlayerRestrictionHeight(float height);
	void									setUseInstancing(int object_type, bool use);
	void									setInstancesTransforms(int object_type, glm::mat4* transforms, int size);
	void									configureInstancedArray(int object_type, GLuint &buffer);
	void									setSoundEnginePointer(ISoundEngine* soundEngine);
	void									addScore(int score);

	int										getScore();
	LevelBehaviour*							getBehaviour();

	int										getObjectsSize();
	GameObject*								getObjectByIndex(int index);
	int										getIndexByObject(GameObject* obj);
	int										getObjectsSizeByType(int obj_type);
	GameObject*								getObjectByTypeIndex(int obj_type, int index);

	std::vector<GameObject*>				getObstaclesInArea(glm::vec4 area, GameObject* excludeObject = NULL);

	glm::vec4								getScreenIndents();
	float									getPlayerRestrictionHeight();

	SpriteRenderer*							getRenderer();

	int										getLevelStatus();

private:
	// holds all objects on scene.
	std::vector<GameObject*>				objects;
	// these objects were created in another level and not have to be deleted.
	std::vector<GameObject*>				externalObjects;
	// pointer to the level behaviour, that controls behaviour of all created objects, that are connected with this level.
	LevelBehaviour*							behaviour = NULL;
	// this object helps us with collision checking. Using for optimization purposes.
	QuadTree*								quadTree = NULL;

	// holds background for level.
	Texture2D*								backgroundCubemap;
	Shader									cubemapShader;

	// this object is using for drawing sprites.
	SpriteRenderer*							renderer = NULL;
	glm::vec4								screenIndents;				// player's moving restrictions.
	float									playerRestrictionHeight;	// player's vertical rectriction.

	// used for instanced drawing.
	// sort all objects by theirs types.
	std::map<int, std::vector<GameObject*>>	typeObjects;
	// holds all instanced objects by types.
	std::map<int, std::vector<GameObject*>>	instancedObjects;
	// holds all other non-instanced objects by types.
	std::map<int, std::vector<GameObject*>>	normalObjects;

	// using in instancing.
	std::map<int, glm::mat4*>				objectsMatrices;
	std::map<int, bool>						useInstancing;

	ISoundEngine*							pSoundEngine = NULL;
	// holds all sound names that can be played on level.
	std::map<std::string, std::string>		soundNames;

	// holds current time, that passed from level starting.
	double									levelTime = 0.0f;
	int										score = 0;								// current player scrore.
	int										levelStatus = LevelStatus::NotStarted;

	// calls when player's score was changed.
	void(*scoreChanged)(int) = NULL;
};
