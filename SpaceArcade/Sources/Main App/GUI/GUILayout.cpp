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

	//for (int i = 0; i < children.size(); ++i)
	//	children[i]->draw();
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
	
	if (typeLayout == GUILayout_Type::Vertical)
		currentSpace = space * screenRatio.x; // not screenRatio.y cause this works better.
	else
		if (typeLayout == GUILayout_Type::Horizontal)
			currentSpace = space * screenRatio.y;

	int totalFills = 0;
	for (int i = 0; i < children.size(); ++i)
		totalFills += children[i]->getLayoutFillPercent();

	for (int i = 0; i < children.size(); ++i)
	{
		if (typeLayout == GUILayout_Type::Vertical)
		{
			if (children[i]->getResizable())
			{
				if (!children[i]->isUseSizeRatio())
				{
					children[i]->setSize(glm::vec2(this->Size.x - (currentIndents.x + currentIndents.z),
						(this->Size.y - (currentIndents.y + currentIndents.w + currentSpace * (children.size() - 1))) *
						children[i]->getLayoutFillPercent() / totalFills));
				}
				else
				{
					float xSize = this->Size.x - (currentIndents.x + currentIndents.z);
					children[i]->setSize(glm::vec2(xSize, xSize / children[i]->getSizeRatio()));
				}
			}
		}
		else if (typeLayout == GUILayout_Type::Horizontal)
		{
			if (children[i]->getResizable())
			{
				if (!children[i]->isUseSizeRatio())
				{
					children[i]->setSize(glm::vec2((this->Size.x - (currentIndents.x + currentIndents.z + currentSpace * (children.size() - 1))) *
						children[i]->getLayoutFillPercent() / totalFills, this->Size.y - (currentIndents.y + currentIndents.w)));
				}
				else
				{
					float ySize = this->Size.y - (currentIndents.y + currentIndents.w);
					children[i]->setSize(glm::vec2(children[i]->getSizeRatio() * ySize, ySize));
				}
			}
		}
	}

	int shift = 0;

	glm::vec2 layout_center = glm::vec2(this->Position.x + this->Size.x / 2, this->Position.y + this->Size.y / 2);
	glm::vec2 content_size = glm::vec2(0.0f, 0.0f);
	for (int i = 0; i < children.size(); ++i)
		content_size += glm::vec2(children[i]->getSize().x + currentSpace, children[i]->getSize().y + currentSpace);
	content_size -= glm::vec2(currentSpace);

	for (int i = 0; i < children.size(); ++i)
	{
		if (typeLayout == GUILayout_Type::Vertical)
		{
			if (alignment == GUILayout_Alignment::Top)
			{
				children[i]->setPosition(glm::vec2(layout_center.x - children[i]->getSize().x / 2, this->Position.y + currentIndents.y + shift));
				shift += children[i]->getSize().y + currentSpace;
			}
			else if (alignment == GUILayout_Alignment::Center)
			{
				children[i]->setPosition(glm::vec2(layout_center.x - children[i]->getSize().x / 2, layout_center.y - content_size.y / 2 + shift));
				shift += children[i]->getSize().y + currentSpace;
			}
		}
		else if(typeLayout == GUILayout_Type::Horizontal)
		{
			if (alignment == GUILayout_Alignment::Left)
				children[i]->setPosition(glm::vec2(this->Position.x + currentIndents.x + shift, layout_center.y - children[i]->getSize().y / 2));
			else if (alignment == GUILayout_Alignment::Center)
				children[i]->setPosition(glm::vec2(layout_center.x - content_size.x / 2 + shift, layout_center.y - children[i]->getSize().y / 2));
			else if (alignment == GUILayout_Alignment::Right)
				children[i]->setPosition(glm::vec2(this->Position.x + this->Size.x - content_size.x + shift, layout_center.y - children[i]->getSize().y / 2));
			
			shift += children[i]->getSize().x + currentSpace;
		}

		//if (dynamic_cast<GUILayout*>(children[i]))
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