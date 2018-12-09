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

#include "GUICheckBox.h"

GUICheckBox::GUICheckBox()
{

}

GUICheckBox::GUICheckBox(SpriteRenderer* renderer, ISoundEngine* soundEngine) : GUIButton(renderer, soundEngine)
{

}

GUICheckBox::~GUICheckBox()
{
	clear();
}

void GUICheckBox::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable)
{
	GUIButton::init(tex, pos, initial_size, resizable);
}

void GUICheckBox::draw()
{
	if (!renderer)
		return;

	GUIButton::draw();
}

void GUICheckBox::resize(bool useParentResize)
{
	GUIButton::resize(useParentResize);

	if (children.size() > 0)
	{
		children[0]->setPosition(this->Position + children[0]->getSourcePosition());
	}
}

bool GUICheckBox::processMouseMove(GLFWwindow* window, float xpos, float ypos)
{
	return GUIButton::processMouseMove(window, xpos, ypos);
}

bool GUICheckBox::processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos)
{
	bool res = GUIButton::processMouseClick(window, button, action, xpos, ypos);

	if (action == GLFW_RELEASE && checkForIntersect(xpos, ypos))
	{
		setChecked(!checked);

		if (checkFunc)
			checkFunc(this->checked);

		return true;
	}

	return res;
}

void GUICheckBox::setChecked(bool checked)
{
	this->checked = checked;
	if (children.size() > 0)
		children[0]->setVisible(checked);
}

void GUICheckBox::setCheckCallback(void(*actionCallback)(bool))
{
	checkFunc = actionCallback;
}

bool GUICheckBox::getChecked()
{
	return checked;
}

void GUICheckBox::clear()
{
	GUIButton::clear();
}