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

	GUITextBox.h
	This class represents simple text box, that can draw some text line without editing.
*/

#include "GUIObject.h"

enum TextAlignment
{
	TextAlignNone = 0,
	TextAlignCenter,
	TextAlignLeft,
	TextAlignRight
};

class GUITextBox : public GUIObject
{
public:
	GUITextBox();
	GUITextBox(SpriteRenderer* renderer, ISoundEngine* soundEngine = NULL);
	virtual ~GUITextBox();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable);

	virtual void draw();
	virtual void resize(bool useParentResize = false);

	virtual void clear();

	void renderText(std::string text, GLfloat scale, glm::vec3 color);

	void setText(std::string text);
	void setFont(Font font);
	void setTextPos(glm::vec2 pos);
	void setTextColor(glm::vec3 color);
	void setTextAlignment(int alignment);
	void setUseClipSpace(bool clipSpace);

	void setFontShader(Shader* shader);
	void setFontBuffers(int vao, int vbo);

	std::string getText();
	Font getFont();
	glm::vec2 getTextPos();
	glm::vec3 getTextColor();

protected:
	std::string text;
	Font font;
	glm::vec2 textRelativePos;							// position relative gui object top left corner.
	glm::vec3 textColor;
	int textAlignment = TextAlignment::TextAlignNone;

	Shader* fontShader = NULL;
	int fontVAO = -1, fontVBO = -1;
};