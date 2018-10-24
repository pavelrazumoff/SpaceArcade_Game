#pragma once
#include "../../Miscellaneous/SpriteRenderer.h"

class GUIObject
{
public:
	GUIObject();
	GUIObject(SpriteRenderer* renderer);
	virtual ~GUIObject();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable);

	virtual void draw();
	virtual void resize(bool useParentResize = false);

	virtual void clear();

	virtual void handleInput(GLFWwindow *window, float delta);
	virtual void processKey(int key, int action);
	virtual void processMouseMove(GLFWwindow* window, float xpos, float ypos);
	virtual void processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos);

	virtual bool checkForIntersect(float xpos, float ypos);

	void addChild(GUIObject* obj);
	void setParent(GUIObject* parent);
	void setPosition(glm::vec2 pos);
	void setSize(glm::vec2 size);

	void setMinimumSize(glm::vec2 size);
	void setMaximumSize(glm::vec2 size);
	void setMinimumWidth(int width);
	void setMinimumHeight(int height);
	void setMaximumWidth(int width);
	void setMaximumHeight(int height);

	void setRenderer(SpriteRenderer* renderer);
	void setVisible(bool visible);
	void setResizable(bool resizable);
	void setUseFixedPosition(bool fixed);
	void setUseSourceResize(bool source);
	void setSizeRatio(float ratio, bool use);			// width / height.
	void setLayoutFillPercent(int percent);

	GUIObject* getParent();
	int getChildrenNum();
	GUIObject* getChildByIndex(int index);
	glm::vec2 getPosition();
	glm::vec2 getSourcePosition();
	glm::vec2 getSize();
	glm::vec2 getSourceSize();
	bool isVisible();
	bool getResizable();
	bool isUseFixedPosition();
	bool isUseSourceResize();
	bool isUseSizeRatio();
	float getSizeRatio();
	int getLayoutFillPercent();

protected:
	glm::vec2 Position, SourcePosition;
	glm::vec2 SourceSize, Size;
	glm::vec2 minimumSize, maximumSize;

	bool visible = true;
	bool resizable = false;
	bool fixedPosition = false;
	bool useSourceResize = true;
	bool useSizeRatio = false;

	float sizeRatio = 1.0;
	int layoutFillPercent = 1;

	Texture2D* Texture = NULL;
	SpriteRenderer* renderer = NULL;

	GUIObject* parent = NULL;
	std::vector<GUIObject*> children;
};