#pragma once
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