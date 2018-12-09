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

#include "GUILayout.h"

GUILayout::GUILayout()
{
	color = glm::vec4(0.0f);
}

GUILayout::GUILayout(SpriteRenderer* renderer) : GUIObject(renderer)
{
	color = glm::vec4(0.0f);
}

GUILayout::~GUILayout()
{
	clear();
}

void GUILayout::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable)
{
	GUIObject::init(tex, pos, initial_size, resizable);
	useSourceResize = false;
}

void GUILayout::draw()
{
	if (!renderer)
		return;

	GUIObject::draw();
}

void GUILayout::resize(bool useParentResize)
{
	if(!useParentResize && !useParentDimensions)
		GUIObject::resize();

	if (useParentDimensions && parent)
	{
		Position = parent->getPosition();
		Size = parent->getSize();
	}

	glm::vec2 screenRatio = renderer->getCurrentScreenDimensions() / renderer->getInitialScreenDimensions();
	glm::vec4 currentIndents = glm::vec4(indents.x * screenRatio.x, indents.y * screenRatio.y, indents.z * screenRatio.x, indents.w * screenRatio.y);
	int currentSpace = space;

	// first we have to get current space between layout children according to the current screen dimensions.
	if (typeLayout == GUILayout_Type::Vertical)
		currentSpace = space * screenRatio.x; // not screenRatio.y cause this works better.
	else
		if (typeLayout == GUILayout_Type::Horizontal)
			currentSpace = space * screenRatio.y;

	// holds sum of all fill percentages of every child.
	int totalFills = 0;
	// this needs to be able to exclude not visible children from layout children calculation positions and dimensions.
	int numOfVisibleChildren = 0;
	for (int i = 0; i < children.size(); ++i)
		if (children[i]->isVisible())
		{
			totalFills += children[i]->getLayoutFillPercent();
			numOfVisibleChildren++;
		}

	int shift = 0;

	// second we have to calculate all children sizes according to the new layout dimensions.
	for (int i = 0; i < children.size(); ++i)
	{
		// if this child is invisible, don't take it into account
		if (!children[i]->isVisible())
			continue;

		if (typeLayout == GUILayout_Type::Vertical)
		{
			// if this child is resizable, set its new size.
			if (children[i]->getResizable())
			{
				float sizeY = (this->Size.y - shift - (currentIndents.y + currentIndents.w + currentSpace * (numOfVisibleChildren - 1))) *
					children[i]->getLayoutFillPercent() / totalFills;

				// if this child doesn't use size ratio, calculate simple sizeX.
				if (!children[i]->isUseSizeRatio())
				{
					children[i]->setSize(glm::vec2(this->Size.x - (currentIndents.x + currentIndents.z), sizeY));

					// if there was some size restrictions with this child, add its size into account and exclude this child from percentage sum.
					if (sizeY != children[i]->getSize().y)
					{
						shift += children[i]->getSize().y;
						totalFills -= children[i]->getLayoutFillPercent();
					}
				}
				else
				{
					// if this child uses size ratio, calculate sizeX according to it.
					float sizeX = this->Size.y * children[i]->getSizeRatio();
					// if sizeX is out of layout bounds, correct it.
					if (sizeX > this->Size.x - (currentIndents.x + currentIndents.z))
					{
						// recalculate sizeY according to new sizeX.
						sizeX = this->Size.x - (currentIndents.x + currentIndents.z);
						sizeY = sizeX / children[i]->getSizeRatio();
					}

					children[i]->setSize(glm::vec2(sizeX, sizeY));

					// don't take this child into percentage account.
					shift += children[i]->getSize().y;
					totalFills -= children[i]->getLayoutFillPercent();
				}
			}
			else
			{
				// don't take this child into percentage account.
				shift += children[i]->getSize().y;
				totalFills -= children[i]->getLayoutFillPercent();
			}
		}
		else if (typeLayout == GUILayout_Type::Horizontal)
		{
			if (children[i]->getResizable())
			{
				float sizeX = (this->Size.x - shift - (currentIndents.x + currentIndents.z + currentSpace * (numOfVisibleChildren - 1))) *
					children[i]->getLayoutFillPercent() / totalFills;
				if (!children[i]->isUseSizeRatio())
				{
					
					children[i]->setSize(glm::vec2(sizeX, this->Size.y - (currentIndents.y + currentIndents.w)));

					if (sizeX != children[i]->getSize().x)
					{
						shift += children[i]->getSize().x;
						totalFills -= children[i]->getLayoutFillPercent();
					}
				}
				else
				{
					float sizeY = sizeX / children[i]->getSizeRatio();
					if (sizeY > this->Size.y - (currentIndents.y + currentIndents.w))
					{
						sizeY = this->Size.y - (currentIndents.y + currentIndents.w);
						sizeX = children[i]->getSizeRatio() * sizeY;
					}

					children[i]->setSize(glm::vec2(sizeX, sizeY));

					shift += children[i]->getSize().x;
					totalFills -= children[i]->getLayoutFillPercent();
				}
			}
			else
			{
				shift += children[i]->getSize().x;
				totalFills -= children[i]->getLayoutFillPercent();
			}
		}
	}

	shift = 0;

	// third we have to positioning all children on current layout.
	glm::vec2 layout_center = glm::vec2(this->Position.x + this->Size.x / 2, this->Position.y + this->Size.y / 2);
	glm::vec2 content_size = glm::vec2(0.0f, 0.0f);

	// calculate content size (it will calculate size for both types of layout, but only one x or y will be used).
	for (int i = 0; i < children.size(); ++i)
		if(children[i]->isVisible())
			content_size += glm::vec2(children[i]->getSize().x + currentSpace, children[i]->getSize().y + currentSpace);
	content_size -= glm::vec2(currentSpace);

	// positioning all children according to the alignment type.
	for (int i = 0; i < children.size(); ++i)
	{
		if (!children[i]->isVisible())
			continue;

		if (typeLayout == GUILayout_Type::Vertical)
		{
			if (alignment == GUILayout_Alignment::Top)
				children[i]->setPosition(glm::vec2(layout_center.x - children[i]->getSize().x / 2, this->Position.y + currentIndents.y + shift));
			else if (alignment == GUILayout_Alignment::Center)
				children[i]->setPosition(glm::vec2(layout_center.x - children[i]->getSize().x / 2, layout_center.y - content_size.y / 2 + shift));
			else if (alignment == GUILayout_Alignment::Bottom)
				children[i]->setPosition(glm::vec2(layout_center.x - children[i]->getSize().x / 2,
					this->Position.y + this->Size.y - content_size.y - currentIndents.w + shift));

			shift += children[i]->getSize().y + currentSpace;
		}
		else if(typeLayout == GUILayout_Type::Horizontal)
		{
			if (alignment == GUILayout_Alignment::Left)
				children[i]->setPosition(glm::vec2(this->Position.x + currentIndents.x + shift, layout_center.y - children[i]->getSize().y / 2));
			else if (alignment == GUILayout_Alignment::Center)
				children[i]->setPosition(glm::vec2(layout_center.x - content_size.x / 2 + shift, layout_center.y - children[i]->getSize().y / 2));
			else if (alignment == GUILayout_Alignment::Right)
				children[i]->setPosition(glm::vec2(this->Position.x + this->Size.x - content_size.x + shift - currentIndents.z,
					layout_center.y - children[i]->getSize().y / 2));
			
			shift += children[i]->getSize().x + currentSpace;
		}

		// after manually calculating every child size, we pointing it that we did this
		// and this object doesn't need to be recalculated again.
		children[i]->resize(true);
	}
}

void GUILayout::setTypeLayout(int type)
{
	typeLayout = type;
}

void GUILayout::setAlignment(int align)
{
	alignment = align;
}

void GUILayout::setSpace(int space)
{
	this->space = space;
}

void GUILayout::setIndents(glm::vec4 indents)
{
	this->indents = indents;
}

void GUILayout::setUseParentDimensions(bool use)
{
	this->useParentDimensions = use;
}

int GUILayout::getTypeLayout()
{
	return typeLayout;
}

int GUILayout::getAlignment()
{
	return alignment;
}

int GUILayout::getSpace()
{
	return space;
}

glm::vec4 GUILayout::getIndents()
{
	return indents;
}

bool GUILayout::isUseParentDimensions()
{
	return useParentDimensions;
}

void GUILayout::clear()
{
	GUIObject::clear();
}