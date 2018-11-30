#pragma once
/*
	GUILayout.h
	This class represents gui vertical and horizontal layout types with
	different alignements. It's not perfect layout realization, but it does basic work as it has to.
*/

#include "GUIObject.h"

enum GUILayout_Type
{
	Vertical = 0,
	Horizontal
};

enum GUILayout_Alignment
{
	Left = 0,
	Right,
	Top,
	Bottom,
	Center,
};

class GUILayout : public GUIObject
{
public:
	GUILayout();
	GUILayout(SpriteRenderer* renderer);
	~GUILayout();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable);

	virtual void draw();
	virtual void resize(bool useParentResize = false);

	virtual void clear();

	void setTypeLayout(int type);
	void setAlignment(int align);
	void setSpace(int space);
	void setIndents(glm::vec4 indents);
	void setUseParentDimensions(bool use);

	int getTypeLayout();
	int getAlignment();
	int getSpace();
	glm::vec4 getIndents();
	bool isUseParentDimensions();

private:
	int typeLayout = GUILayout_Type::Vertical;
	int alignment = GUILayout_Alignment::Top;
	int space = 0;					// vertical or horizontal space between two adjacent objects.
	glm::vec4 indents;				// indents on the sides of layout frame.

	// true - this layout dimensions is taken from its parent.
	bool useParentDimensions = false;
};