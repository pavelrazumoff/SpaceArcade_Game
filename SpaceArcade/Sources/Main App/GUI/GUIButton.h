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