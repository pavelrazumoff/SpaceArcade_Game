#pragma once
#include "GUIObject.h"

enum GUILayout_Alignment
{
	Vertical = 0,
	Horizontal
};

class GUILayout : public GUIObject
{
public:
	GUILayout();
	GUILayout(SpriteRenderer* renderer);
	~GUILayout();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_scale, bool resizable);

	virtual void draw();
	virtual void resize();

	virtual void clear();

	void setAlignment(int align);
	void setSpace(int space);
	void setIndents(glm::vec4 indents);

	int getAlignment();
	int getSpace();
	glm::vec4 getIndents();

private:
	int alignment = GUILayout_Alignment::Vertical;
	int space = 0;					// vertical or horizontal space between two adjacent objects.
	glm::vec4 indents;				// indents on the sides of layout frame.
};