#include "GUIObject.h"

GUIObject::GUIObject() : minimumSize(glm::vec2(0)), maximumSize(glm::vec2(1000000))
{
}

GUIObject::GUIObject(SpriteRenderer* renderer) : minimumSize(glm::vec2(0)), maximumSize(glm::vec2(1000000))
{
	this->renderer = renderer;
}

GUIObject::~GUIObject()
{
	clear();
}

void GUIObject::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable)
{
	this->Texture = tex;
	this->Position = pos;
	this->SourceSize = initial_size;
	this->Size = initial_size;
	this->resizable = resizable;
}

void GUIObject::draw()
{
	if(renderer)
		renderer->DrawSprite(Texture, Position, Size);

	for (int i = 0; i < children.size(); ++i)
		children[i]->draw();
}

void GUIObject::resize(bool useParentResize)
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
		if (useSourceResize)
		{
			glm::vec2 screenRatio = renderer->getCurrentScreenDimensions() / renderer->getInitialScreenDimensions();
			if (!useSizeRatio)
				Size = glm::vec2(SourceSize.x * screenRatio.x, SourceSize.y * screenRatio.x); // always save source scale ratio.
			else
				Size = glm::vec2(SourceSize.x * screenRatio.x, (SourceSize.x * screenRatio.x) / sizeRatio);
		}
		else
		{
			glm::vec2 screenRatio = renderer->getScreenRatio();
			if (!useSizeRatio)
				Size *= screenRatio;
			else
				Size = glm::vec2(Size.x * screenRatio.x, (Size.x * screenRatio.x) / sizeRatio);
		}
	}

	for (int i = 0; i < children.size(); ++i)
		children[i]->resize();
}

void GUIObject::handleInput(GLFWwindow *window, float delta)
{
	for (int i = 0; i < children.size(); ++i)
		children[i]->handleInput(window, delta);
}

void GUIObject::processKey(int key, int action)
{
	for (int i = 0; i < children.size(); ++i)
		children[i]->processKey(key, action);
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
	if (xpos >= Position.x && xpos <= Position.x + Size.x &&
		ypos >= Position.y && ypos <= Position.y + Size.y)
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

void GUIObject::setSize(glm::vec2 size)
{
	Size = size;
	if (Size.x < minimumSize.x)
		Size.x = minimumSize.x;

	if (Size.x > maximumSize.x)
		Size.x = maximumSize.x;

	if (Size.y < minimumSize.y)
		Size.y = minimumSize.y;

	if (Size.y > maximumSize.y)
		Size.y = maximumSize.y;

	if (useSizeRatio)
		Size.y = Size.x / sizeRatio;
}

void GUIObject::setMinimumSize(glm::vec2 size)
{
	minimumSize = size;
}

void GUIObject::setMaximumSize(glm::vec2 size)
{
	maximumSize = size;
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

void GUIObject::setUseSourceResize(bool source)
{
	this->useSourceResize = source;
}

void GUIObject::setSizeRatio(float ratio, bool use)
{
	this->useSizeRatio = use;
	sizeRatio = ratio;
}

void GUIObject::setLayoutFillPercent(int percent)
{
	this->layoutFillPercent = percent;
}

GUIObject* GUIObject::getParent()
{
	return parent;
}

int GUIObject::getChildrenNum()
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

glm::vec2 GUIObject::getSourceSize()
{
	return SourceSize;
}

glm::vec2 GUIObject::getSize()
{
	return Size;
}

bool GUIObject::getResizable()
{
	return resizable;
}

bool GUIObject::isUseFixedPosition()
{
	return fixedPosition;
}

bool GUIObject::isUseSourceResize()
{
	return useSourceResize;
}

bool GUIObject::isUseSizeRatio()
{
	return useSizeRatio;
}

float GUIObject::getSizeRatio()
{
	return sizeRatio;
}

int GUIObject::getLayoutFillPercent()
{
	return layoutFillPercent;
}

void GUIObject::clear()
{
	for (int i = 0; i < children.size(); ++i)
		delete children[i];
	children.clear();
}