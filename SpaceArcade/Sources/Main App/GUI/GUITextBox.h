#pragma once
#include "GUIObject.h"

class GUITextBox : public GUIObject
{
public:
	GUITextBox();
	GUITextBox(SpriteRenderer* renderer);
	virtual ~GUITextBox();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable);

	virtual void draw();
	virtual void resize(bool useParentResize = false);

	virtual void clear();

	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	void setText(std::string text);
	void setFont(std::map<GLchar, Character> font);
	void setTextPos(glm::vec2 pos);
	void setTextColor(glm::vec3 color);
	void setFontShader(Shader* shader);
	void setFontBuffers(int vao, int vbo);

	std::string getText();
	std::map<GLchar, Character> getFont();
	glm::vec2 getTextPos();
	glm::vec3 getTextColor();

protected:
	std::string text;
	std::map<GLchar, Character> font;
	glm::vec2 textRelativePos;
	glm::vec3 textColor;

	Shader* fontShader = NULL;
	int fontVAO = -1, fontVBO = -1;
};