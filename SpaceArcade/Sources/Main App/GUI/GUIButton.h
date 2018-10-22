#pragma once
#include "GUIObject.h"

enum GUI_ButtonState
{
	DefaultState = 0,
	HoveredState,
	PressedState,
	CheckedState
};

class GUIButton : public GUIObject
{
public:
	GUIButton();
	GUIButton(SpriteRenderer* renderer);
	virtual ~GUIButton();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_scale, bool resizable);

	virtual void draw();
	virtual void resize();

	virtual void clear();

	virtual void processMouseMove(GLFWwindow* window, float xpos, float ypos);
	virtual void processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos);

	void setHoveredTexture(Texture2D* tex);
	void setPressedTexture(Texture2D* tex);

	void setActionCallback(void(*actionCallback)(void));

protected:
	Texture2D* hoveredTexture = NULL;
	Texture2D* pressedTexture = NULL;

	int buttonState = GUI_ButtonState::DefaultState;			// 0 - default, 1 - hovered, 2 - pressed.
	bool checkable = false;

	void(*actionFunc)(void) = NULL;
};