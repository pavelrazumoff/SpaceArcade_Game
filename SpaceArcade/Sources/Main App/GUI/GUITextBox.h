#pragma once
/*
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