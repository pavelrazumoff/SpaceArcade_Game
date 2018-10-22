#include "GUILayout.h"

GUILayout::GUILayout()
{

}

GUILayout::GUILayout(SpriteRenderer* renderer) : GUIObject(renderer)
{

}

GUILayout::~GUILayout()
{
	clear();
}

void GUILayout::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_scale, bool resizable)
{
	GUIObject::init(tex, pos, initial_scale, resizable);
}

void GUILayout::draw()
{
	if (!renderer)
		return;

	for (int i = 0; i < children.size(); ++i)
		children[i]->draw();
}

void GUILayout::resize()
{
	GUIObject::resize();

	glm::vec2 screenRatio = renderer->getCurrentScreenDimensions() / renderer->getInitialScreenDimensions();
	glm::vec4 currentIndents = glm::vec4(indents.x * screenRatio.x, indents.y * screenRatio.y, indents.z * screenRatio.x, indents.w * screenRatio.y);
	int currentSpace = space;
	
	if (alignment == GUILayout_Alignment::Vertical)
		currentSpace = space * screenRatio.x; // not screenRatio.y cause this works better.
	else
		if (alignment == GUILayout_Alignment::Horizontal)
			currentSpace = space * screenRatio.y;

	for (int i = 0; i < children.size(); ++i)
	{
		if (alignment == GUILayout_Alignment::Vertical)
		{
			if (children[i]->getResizable())
			{
				children[i]->setScale(glm::vec2(this->Scale.x - (currentIndents.x + currentIndents.z),
					(this->Scale.y - (currentIndents.y + currentIndents.w + currentSpace * (children.size() - 1))) / children.size()));
			}
		}
		else if (alignment == GUILayout_Alignment::Horizontal)
		{
			if (children[i]->getResizable())
			{
				children[i]->setScale(glm::vec2((this->Scale.x - (currentIndents.x + currentIndents.z + currentSpace * (children.size() - 1))) / children.size(),
					this->Scale.y - (currentIndents.y + currentIndents.w)));
			}
		}
	}

	int shift = 0;

	for (int i = 0; i < children.size(); ++i)
	{
		if (alignment == GUILayout_Alignment::Vertical)
		{
			children[i]->setPosition(glm::vec2(this->Position.x + currentIndents.x, this->Position.y + currentIndents.y + shift));
			shift += children[i]->getScale().y + currentSpace;
		}
		else if(alignment == GUILayout_Alignment::Horizontal)
		{
			children[i]->setPosition(glm::vec2(this->Position.x + currentIndents.x + shift, this->Position.y + currentIndents.y));
			shift += children[i]->getScale().x + currentSpace;
		}
	}
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

void GUILayout::clear()
{
}