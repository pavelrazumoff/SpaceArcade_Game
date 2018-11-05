#pragma once
#include "SpacecraftObject.h"
#include "EnergyBarrierObject.h"
#include "../../Miscellaneous/QuadTree.h"

class LevelBehaviour;

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
	void									updateInstances();
	void									startLevel();			// calls once after all initializations and before updating.

	void									handleInput(GLFWwindow *window, float delta);
	void									processKey(int key, int action, bool* key_pressed);

	void									doCollisions();

	void									addNewObject(GameObject* obj);
	void									removeObject(GameObject* obj);

	void									addSound(std::string soundName, std::string soundPath);
	ISound*									playSound(std::string soundName, bool loop);

	void									setBehaviour(LevelBehaviour* behaviour);
	void									setScreenIndents(glm::vec4 indents);
	void									setPlayerRestrictionHeight(float height);
	void									setUseInstancing(int object_type, bool use);
	void									setInstancesTransforms(int object_type, glm::mat4* transforms, int size);
	void									setSoundEnginePointer(ISoundEngine* soundEngine);

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

	bool									isStarted();

private:
	std::vector<GameObject*>				objects;
	LevelBehaviour*							behaviour = NULL;
	QuadTree*								quadTree = NULL;

	Texture2D*								backgroundCubemap;
	Shader									cubemapShader;

	SpriteRenderer*							renderer = NULL;
	glm::vec4								screenIndents;
	float									playerRestrictionHeight;

	// used for instanced drawing.
	std::map<int, std::vector<GameObject*>>	typeObjects;
	std::map<int, std::vector<GameObject*>>	instancedObjects;
	std::map<int, std::vector<GameObject*>>	normalObjects;

	std::map<int, glm::mat4*>				objectsMatrices;
	std::map<int, bool>						useInstancing;

	ISoundEngine*							pSoundEngine = NULL;
	std::map<std::string, std::string>		soundNames;

	bool									wasStarted = false;
};
