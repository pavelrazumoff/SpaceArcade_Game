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

void GUICheckBox::processMouseMove(GLFWwindow* window, float xpos, float ypos)
{
	GUIButton::processMouseMove(window, xpos, ypos);
}

void GUICheckBox::processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos)
{
	GUIButton::processMouseClick(window, button, action, xpos, ypos);

	if (action == GLFW_RELEASE && checkForIntersect(xpos, ypos))
	{
		setChecked(!checked);

		if (checkFunc)
			checkFunc(this->checked);
	}
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