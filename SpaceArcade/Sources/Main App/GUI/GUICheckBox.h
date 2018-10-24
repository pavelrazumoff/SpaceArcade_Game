#pragma once
#include "GUIButton.h"

class GUICheckBox : public GUIButton
{
public:
	GUICheckBox();
	GUICheckBox(SpriteRenderer* renderer);
	virtual ~GUICheckBox();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable);

	virtual void draw();
	virtual void resize(bool useParentResize = false);

	virtual void clear();

	virtual void processMouseMove(GLFWwindow* window, float xpos, float ypos);
	virtual void processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos);

	void setChecked(bool checked);

	bool getChecked();

	void setCheckCallback(void(*actionCallback)(bool));

protected:
	bool checked = false;

	void(*checkFunc)(bool) = NULL;
};