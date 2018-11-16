#include "GUITextBox.h"

GUITextBox::GUITextBox()
{
	color = glm::vec4(0.0f);
}

GUITextBox::GUITextBox(SpriteRenderer* renderer, ISoundEngine* soundEngine) : GUIObject(renderer, soundEngine)
{
	color = glm::vec4(0.0f);
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

	renderText(this->text, this->Position.x + textRelativePos.x, this->Position.y + textRelativePos.y, 1.0f, textColor);
}

void GUITextBox::resize(bool useParentResize)
{
	GUIObject::resize(useParentResize);
}

void GUITextBox::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
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
	fontShader->setBool("useClipSpace", true);
	fontShader->setVec4("clipSpace", glm::vec4(Position.x, Position.y, Position.x + Size.x, Position.y + Size.y));

	glUniform3f(glGetUniformLocation(fontShader->getShaderProgram(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(fontVAO);
	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = font[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y + (ch.Size.y - ch.Bearing.y) * scale;
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
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
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

void GUITextBox::setFont(std::map<GLchar, Character> font)
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

std::map<GLchar, Character> GUITextBox::getFont()
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