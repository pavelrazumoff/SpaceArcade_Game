#include "GUIObject.h"

GUIObject::GUIObject()
{
}

GUIObject::GUIObject(SpriteRenderer* renderer)
{
	this->renderer = renderer;
}

GUIObject::~GUIObject()
{
	clear();
}

void GUIObject::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_scale, bool resizable)
{
	this->Texture = tex;
	this->Position = pos;
	this->SourceScale = initial_scale;
	this->Scale = initial_scale;
	this->resizable = resizable;
}

void GUIObject::draw()
{
	if(renderer)
		renderer->DrawSprite(Texture, Position, Scale);

	for (int i = 0; i < children.size(); ++i)
		children[i]->draw();
}

void GUIObject::resize()
{
	if (!renderer)
		return;

	if (!fixedPosition)
	{
		glm::vec2 screenRatio = renderer->getScreenRatio();
		Position = glm::vec2(Position.x * screenRatio.x, Position.y * screenRatio.y);
	}

	if (resizable)
	{
		glm::vec2 screenRatio = renderer->getCurrentScreenDimensions() / renderer->getInitialScreenDimensions();
		Scale = glm::vec2(SourceScale.x * screenRatio.x, SourceScale.y * screenRatio.x); // always save source scale ratio.
	}

	for (int i = 0; i < children.size(); ++i)
		children[i]->resize();
}

void GUIObject::handleInput(GLFWwindow *window, float delta)
{
	for (int i = 0; i < children.size(); ++i)
		children[i]->handleInput(window, delta);
}

void GUIObject::processKey(int key, int action, bool* key_pressed)
{
	for (int i = 0; i < children.size(); ++i)
		children[i]->processKey(key, action, key_pressed);
}

void GUIObject::processMouseMove(GLFWwindow* window, float xpos, float ypos)
{
	for (int i = 0; i < children.size(); ++i)
		children[i]->processMouseMove(window, xpos, ypos);
}

void GUIObject::processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos)
{
	for (int i = 0; i < children.size(); ++i)
		children[i]->processMouseClick(window, button, action, xpos, ypos);
}

bool GUIObject::checkForIntersect(float xpos, float ypos)
{
	if (xpos >= Position.x && xpos <= Position.x + Scale.x &&
		ypos >= Position.y && ypos <= Position.y + Scale.y)
		return true;
	return false;
}

void GUIObject::addChild(GUIObject* obj)
{
	std::vector<GUIObject*>::iterator it;

	it = find(children.begin(), children.end(), obj);
	if (it == children.end())
	{
		children.push_back(obj);
		obj->setParent(this);
	}
}

void GUIObject::setParent(GUIObject* parent)
{
	this->parent = parent;
}

void GUIObject::setPosition(glm::vec2 pos)
{
	Position = pos;
}

void GUIObject::setScale(glm::vec2 scale)
{
	Scale = scale;
}

void GUIObject::setRenderer(SpriteRenderer* renderer)
{
	this->renderer = renderer;
}

void GUIObject::setResizable(bool resizable)
{
	this->resizable = resizable;
}

void GUIObject::setUseFixedPosition(bool fixed)
{
	this->fixedPosition = fixed;
}

GUIObject* GUIObject::getParent()
{
	return parent;
}

int GUIObject::getChildrenSize()
{
	return children.size();
}

GUIObject* GUIObject::getChildByIndex(int index)
{
	if (index < 0 || index >= children.size())
		return NULL;
	return children[index];
}

glm::vec2 GUIObject::getPosition()
{
	return Position;
}

glm::vec2 GUIObject::getSourceScale()
{
	return SourceScale;
}

glm::vec2 GUIObject::getScale()
{
	return Scale;
}

bool GUIObject::getResizable()
{
	return resizable;
}

bool GUIObject::isUseFixedPosition()
{
	return fixedPosition;
}

void GUIObject::clear()
{
	for (int i = 0; i < children.size(); ++i)
		delete children[i];
	children.clear();
}