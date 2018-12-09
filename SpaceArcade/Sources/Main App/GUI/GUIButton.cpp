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
*/

#include "GUIButton.h"

GUIButton::GUIButton()
{

}

GUIButton::GUIButton(SpriteRenderer* renderer, ISoundEngine* soundEngine) : GUIObject(renderer, soundEngine)
{

}

GUIButton::~GUIButton()
{
	clear();
}

void GUIButton::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable)
{
	GUIObject::init(tex, pos, initial_size, resizable);
}

void GUIButton::draw()
{
	if (!renderer || !visible)
		return;

	switch (buttonState)
	{
	case GUI_ButtonState::DefaultState:
		renderer->DrawSprite(Texture, Position, Size);
		break;
	case GUI_ButtonState::HoveredState:
		renderer->DrawSprite(hoveredTexture, Position, Size);
		break;
	case GUI_ButtonState::PressedState:
		renderer->DrawSprite(pressedTexture, Position, Size);
		break;
	default:
		break;
	}

	for (int i = 0; i < children.size(); ++i)
		children[i]->draw();
}

void GUIButton::resize(bool useParentResize)
{
	GUIObject::resize(useParentResize);
}

bool GUIButton::processMouseMove(GLFWwindow* window, float xpos, float ypos)
{
	if (checkForIntersect(xpos, ypos))
	{
		switch (buttonState)
		{
		case GUI_ButtonState::DefaultState:
			playSound("ClickEffect", false);
			buttonState = GUI_ButtonState::HoveredState;
			break;
		default:
			break;
		}

		return true;
	}
	else
	{
		switch (buttonState)
		{
		case GUI_ButtonState::HoveredState:
			buttonState = GUI_ButtonState::DefaultState;
			break;
		default:
			break;
		}
	}

	return false;
}

bool GUIButton::processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS && checkForIntersect(xpos, ypos))
		{
			if (!checkable)
				buttonState = GUI_ButtonState::PressedState;

			return true;
		}
		else if (action == GLFW_RELEASE)
		{
			if (!checkable)
			{
				if (checkForIntersect(xpos, ypos))
				{
					buttonState = GUI_ButtonState::HoveredState;

					if (actionFunc)
						actionFunc(actionId);
					return true;
				}
				else
					buttonState = GUI_ButtonState::DefaultState;
			}
		}
	}

	return false;
}

void GUIButton::setHoveredTexture(Texture2D* tex)
{
	hoveredTexture = tex;
}

void GUIButton::setPressedTexture(Texture2D* tex)
{
	pressedTexture = tex;
}

void GUIButton::setActionCallback(void(*actionCallback)(int))
{
	actionFunc = actionCallback;
}

void GUIButton::setActionID(int id)
{
	actionId = id;
}

int GUIButton::getActionID()
{
	return actionId;
}

void GUIButton::clear()
{
	GUIObject::clear();
}