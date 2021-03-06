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

#include "GUIObject.h"

GUIObject::GUIObject() : minimumSize(glm::vec2(0)), maximumSize(glm::vec2(1000000))
{
}

GUIObject::GUIObject(SpriteRenderer* renderer, ISoundEngine* soundEngine) : minimumSize(glm::vec2(0)), maximumSize(glm::vec2(1000000))
{
	this->renderer = renderer;
	this->pSoundEngine = soundEngine;
}

GUIObject::~GUIObject()
{
	clear();
}

void GUIObject::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable)
{
	this->Texture = tex;
	this->Position = pos;
	this->SourcePosition = pos;
	this->Size = initial_size;
	this->SourceSize = initial_size;
	this->resizable = resizable;
}

void GUIObject::draw()
{
	if (!visible || !renderer)
		return;

	// set clip space.
	Shader* pShader = renderer->getShader();
	glm::vec4 realClip = glm::vec4(Position.x + Size.x * clipSpace.x, Position.y + Size.y * clipSpace.y,
		Position.x + Size.x - (Size.x * clipSpace.z), Position.y + Size.y - (Size.y * clipSpace.w));

	pShader->setBool("useClipSpace", useClipSpace);
	pShader->setVec4("clipSpace", realClip);

	renderer->DrawSprite(Texture, Position, Size, color);

	// restore default.
	pShader->setBool("useClipSpace", false);

	for (int i = 0; i < children.size(); ++i)
		children[i]->draw();
}

void GUIObject::resize(bool useParentResize)
{
	if (!renderer)
		return;

	if (!useParentResize)
	{
		if (!fixedPosition)
		{
			glm::vec2 screenRatio = renderer->getCurrentScreenDimensions() / renderer->getInitialScreenDimensions();
			Position = glm::vec2(SourcePosition.x * screenRatio.x, SourcePosition.y * screenRatio.y);

			if (parent)
				Position = parent->getPosition() + Position;
		}
		else
		{
			if (parent)
				Position = parent->getPosition() + SourcePosition;
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
	}

	for (int i = 0; i < children.size(); ++i)
		children[i]->resize();
}

bool GUIObject::handleInput(GLFWwindow *window, float delta)
{
	for (int i = 0; i < children.size(); ++i)
		if (children[i]->isVisible())
			if (children[i]->handleInput(window, delta))
				return true;
	return false;
}

bool GUIObject::processKey(int key, int action)
{
	for (int i = 0; i < children.size(); ++i)
		if (children[i]->isVisible())
			if(children[i]->processKey(key, action))
				return true;
	return false;
}

bool GUIObject::processMouseMove(GLFWwindow* window, float xpos, float ypos)
{
	for (int i = 0; i < children.size(); ++i)
		if (children[i]->isVisible())
			if(children[i]->processMouseMove(window, xpos, ypos))
				return true;
	return false;
}

bool GUIObject::processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos)
{
	for (int i = 0; i < children.size(); ++i)
		if (children[i]->isVisible())
			if (children[i]->processMouseClick(window, button, action, xpos, ypos))
				return true;
	return false;
}

void GUIObject::addSound(std::string soundName, std::string soundPath)
{
	soundNames[soundName] = soundPath;
}

ISound* GUIObject::playSound(std::string soundName, bool loop)
{
	auto it = soundNames.find(soundName);
	if (it != soundNames.end())
		return pSoundEngine->play2D(soundNames[soundName].c_str(), loop, false, true);
	return NULL;
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

void GUIObject::setMinimumWidth(int width)
{
	minimumSize.x = width;
}

void GUIObject::setMinimumHeight(int height)
{
	minimumSize.y = height;
}

void GUIObject::setMaximumWidth(int width)
{
	maximumSize.x = width;
}

void GUIObject::setMaximumHeight(int height)
{
	maximumSize.y = height;
}

void GUIObject::setTexture(Texture2D* tex)
{
	Texture = tex;
}

void GUIObject::setRenderer(SpriteRenderer* renderer)
{
	this->renderer = renderer;
}

void GUIObject::setVisible(bool visible)
{
	this->visible = visible;
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

void GUIObject::setClipSpace(glm::vec4 clip, bool use)
{
	useClipSpace = use;
	clipSpace = clip;
}

void GUIObject::setColor(glm::vec4 color)
{
	this->color = color;
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

glm::vec2 GUIObject::getSourcePosition()
{
	return SourcePosition;
}

glm::vec2 GUIObject::getSize()
{
	return Size;
}

glm::vec2 GUIObject::getSourceSize()
{
	return SourceSize;
}

glm::vec4 GUIObject::getClipSpace()
{
	return clipSpace;
}

bool GUIObject::isVisible()
{
	return visible;
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

bool GUIObject::isUseClipSpace()
{
	return useClipSpace;
}

float GUIObject::getSizeRatio()
{
	return sizeRatio;
}

int GUIObject::getLayoutFillPercent()
{
	return layoutFillPercent;
}

glm::vec4 GUIObject::getColor()
{
	return color;
}

glm::vec2 GUIObject::getMaximumSize()
{
	return maximumSize;
}

glm::vec2 GUIObject::getMinimumSize()
{
	return minimumSize;
}

int GUIObject::getMaximumWidth()
{
	return maximumSize.x;
}

int GUIObject::getMaximumHeight()
{
	return maximumSize.y;
}

int GUIObject::getMinimumWidth()
{
	return minimumSize.x;
}

int GUIObject::getMinimumHeight()
{
	return minimumSize.y;
}

void GUIObject::clear()
{
	for (int i = 0; i < children.size(); ++i)
		delete children[i];
	children.clear();
}