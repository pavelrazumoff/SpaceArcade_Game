#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

class GUIObject
{
public:
	GUIObject();
	GUIObject(SpriteRenderer* renderer);
	virtual ~GUIObject();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_scale, bool resizable);

	virtual void draw();
	virtual void resize();

	virtual void clear();

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action, bool* key_pressed);
	virtual void processMouseMove(GLFWwindow* window, float xpos, float ypos);
	virtual void processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos);

	virtual bool checkForIntersect(float xpos, float ypos);

	void addChild(GUIObject* obj);
	void setParent(GUIObject* parent);
	void setPosition(glm::vec2 pos);
	void setScale(glm::vec2 scale);
	void setRenderer(SpriteRenderer* renderer);
	void setResizable(bool resizable);
	void setUseFixedPosition(bool fixed);

	GUIObject* getParent();
	int getChildrenSize();
	GUIObject* getChildByIndex(int index);
	glm::vec2 getPosition();
	glm::vec2 getSourceScale();
	glm::vec2 getScale();
	bool getResizable();
	bool isUseFixedPosition();

protected:
	glm::vec2 Position;
	glm::vec2 SourceScale, Scale;

	bool resizable = false;
	bool fixedPosition = false;

	Texture2D* Texture = NULL;
	SpriteRenderer* renderer = NULL;

	GUIObject* parent = NULL;
	std::vector<GUIObject*> children;
};