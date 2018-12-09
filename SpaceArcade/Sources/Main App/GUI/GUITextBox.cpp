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

#include "GUITextBox.h"

GUITextBox::GUITextBox()
{
	color = glm::vec4(0.0f);
	useClipSpace = true;
}

GUITextBox::GUITextBox(SpriteRenderer* renderer, ISoundEngine* soundEngine) : GUIObject(renderer, soundEngine)
{
	color = glm::vec4(0.0f);
	useClipSpace = true;
}

GUITextBox::~GUITextBox()
{
	clear();
}

void GUITextBox::init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable)
{
	GUIObject::init(tex, pos, initial_size, resizable);
}

void GUITextBox::draw()
{
	if (!renderer)
		return;

	GUIObject::draw();

	renderText(this->text, 1.0f, textColor);
}

void GUITextBox::resize(bool useParentResize)
{
	GUIObject::resize(useParentResize);
}

void GUITextBox::renderText(std::string text, GLfloat scale, glm::vec3 color)
{
	if (!fontShader || fontVAO < 0 || fontVBO < 0)
		return;

	GLint prevBlendFunc;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &prevBlendFunc);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate corresponding render state
	fontShader->use();

	glm::vec2 screenDimensions = renderer->getCurrentScreenDimensions();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenDimensions.x), static_cast<GLfloat>(screenDimensions.y), 0.0f, -1.0f, 1.0f);

	fontShader->setMat4("projection", projection);
	fontShader->setBool("useClipSpace", useClipSpace);
	fontShader->setVec4("clipSpace", glm::vec4(Position.x, Position.y, Position.x + Size.x, Position.y + Size.y));

	glUniform3f(glGetUniformLocation(fontShader->getShaderProgram(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(fontVAO);

	// first we have to determine text width.
	float textWidth = 0;
	std::string::const_iterator c;
	float x = 0, y = 0;

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = font.characters[*c];
		x += (ch.Advance >> 6) * scale;
	}

	textWidth = x;

	// second we have to set text position with appropriate alignment.
	glm::vec2 textPos;

	switch (textAlignment)
	{
	case TextAlignment::TextAlignNone:
	case TextAlignment::TextAlignLeft:
		textPos = glm::vec2(this->Position.x, this->Position.y);
		break;
	case TextAlignment::TextAlignCenter:
		textPos = glm::vec2(this->Position.x + this->Size.x / 2 - textWidth / 2, this->Position.y);
		break;
	case TextAlignment::TextAlignRight:
		textPos = glm::vec2(this->Position.x + this->Size.x - textWidth, this->Position.y);
		break;
	default:
		break;
	}

	textPos += textRelativePos;

	// Iterate through all characters
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = font.characters[*c];
		GLfloat xpos = textPos.x + ch.Bearing.x * scale;
		GLfloat ypos = textPos.y + font.fontSize + (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos, ypos - h, 0.0, 0.0 },
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos, ypos - h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos - h, 1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		textPos.x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBlendFunc(GL_SRC_ALPHA, prevBlendFunc);
	glDisable(GL_BLEND);
}

void GUITextBox::setText(std::string text)
{
	this->text = text;
}

void GUITextBox::setFont(Font font)
{
	this->font = font;
}

void GUITextBox::setTextPos(glm::vec2 pos)
{
	textRelativePos = pos;
}

void GUITextBox::setTextColor(glm::vec3 color)
{
	textColor = color;
}

void GUITextBox::setTextAlignment(int alignment)
{
	textAlignment = alignment;
}

void GUITextBox::setUseClipSpace(bool clipSpace)
{
	useClipSpace = clipSpace;
}

void GUITextBox::setFontShader(Shader* shader)
{
	fontShader = shader;
}

void GUITextBox::setFontBuffers(int vao, int vbo)
{
	fontVAO = vao;
	fontVBO = vbo;
}

std::string GUITextBox::getText()
{
	return text;
}

Font GUITextBox::getFont()
{
	return font;
}

glm::vec2 GUITextBox::getTextPos()
{
	return textRelativePos;
}

glm::vec3 GUITextBox::getTextColor()
{
	return textColor;
}

void GUITextBox::clear()
{
	GUIObject::clear();
}