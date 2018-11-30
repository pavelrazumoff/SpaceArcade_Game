#pragma once
/*
	GUIButton.h
	This class represents gui button with different button states.
	Every button states presented with specific texture.
*/

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
	GUIButton(SpriteRenderer* renderer, ISoundEngine* soundEngine = NULL);
	virtual ~GUIButton();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable);

	virtual void draw();
	virtual void resize(bool useParentResize = false);

	virtual void clear();

	virtual bool processMouseMove(GLFWwindow* window, float xpos, float ypos);
	virtual bool processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos);

	void setHoveredTexture(Texture2D* tex);
	void setPressedTexture(Texture2D* tex);

	void setActionCallback(void(*actionCallback)(int));
	void setActionID(int id);

	int getActionID();

protected:
	Texture2D* hoveredTexture = NULL;
	Texture2D* pressedTexture = NULL;

	int buttonState = GUI_ButtonState::DefaultState;			// 0 - default, 1 - hovered, 2 - pressed.
	bool checkable = false;										// true - when button is clicked, it stays clicked until it won't be unclicked.

	int actionId = -1;											// holds action type, that is defined in external code.
	void(*actionFunc)(int) = NULL;								// calls when button state is changing.
};