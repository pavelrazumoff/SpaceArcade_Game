#include "GUIButton.h"

GUIButton::GUIButton()
{

}

GUIButton::GUIButton(SpriteRenderer* renderer) : GUIObject(renderer)
{

}

GUIButton::~GUIButton()
{
	clear();
}

void GUIButton::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_scale, bool resizable)
{
	GUIObject::init(tex, pos, initial_scale, resizable);
}

void GUIButton::draw()
{
	if (!renderer)
		return;

	switch (buttonState)
	{
	case GUI_ButtonState::DefaultState:
		renderer->DrawSprite(Texture, Position, Scale);
		break;
	case GUI_ButtonState::HoveredState:
		renderer->DrawSprite(hoveredTexture, Position, Scale);
		break;
	case GUI_ButtonState::PressedState:
		renderer->DrawSprite(pressedTexture, Position, Scale);
		break;
	default:
		break;
	}
}

void GUIButton::resize()
{
	GUIObject::resize();
}

void GUIButton::processMouseMove(GLFWwindow* window, float xpos, float ypos)
{
	if (checkForIntersect(xpos, ypos))
	{
		switch (buttonState)
		{
		case GUI_ButtonState::DefaultState:
			buttonState = GUI_ButtonState::HoveredState;
			break;
		default:
			break;
		}
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
}

void GUIButton::processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS && checkForIntersect(xpos, ypos))
		{
			if (!checkable)
				buttonState = GUI_ButtonState::PressedState;

			if (actionFunc)
				actionFunc();
		}
		else if (action == GLFW_RELEASE)
		{
			if (!checkable)
			{
				if (checkForIntersect(xpos, ypos))
					buttonState = GUI_ButtonState::HoveredState;
				else
					buttonState = GUI_ButtonState::DefaultState;
			}
		}
	}
}

void GUIButton::setHoveredTexture(Texture2D* tex)
{
	hoveredTexture = tex;
}

void GUIButton::setPressedTexture(Texture2D* tex)
{
	pressedTexture = tex;
}

void GUIButton::setActionCallback(void(*actionCallback)(void))
{
	actionFunc = actionCallback;
}

void GUIButton::clear()
{
	
}