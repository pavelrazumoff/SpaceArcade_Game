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

	GUIObject.h
	This class represents basic gui object, that hasn't any of interactivity,
	but can be rendered on screen with specified size and position.
*/

#include "../../Miscellaneous/SpriteRenderer.h"

class GUIObject
{
public:
	GUIObject();
	GUIObject(SpriteRenderer* renderer, ISoundEngine* soundEngine = NULL);
	virtual ~GUIObject();

	virtual void init(Texture2D* tex, glm::vec2 pos, glm::vec2 initial_size, bool resizable);

	virtual void draw();
	virtual void resize(bool useParentResize = false);

	virtual void clear();

	virtual bool handleInput(GLFWwindow *window, float delta);
	virtual bool processKey(int key, int action);
	virtual bool processMouseMove(GLFWwindow* window, float xpos, float ypos);
	virtual bool processMouseClick(GLFWwindow* window, int button, int action, float xpos, float ypos);

	void addSound(std::string soundName, std::string soundPath);
	ISound*	playSound(std::string soundName, bool loop);

	virtual bool checkForIntersect(float xpos, float ypos);

	void addChild(GUIObject* obj);
	void setParent(GUIObject* parent);
	void setPosition(glm::vec2 pos);
	void setSize(glm::vec2 size);

	void setMinimumSize(glm::vec2 size);
	void setMaximumSize(glm::vec2 size);
	void setMinimumWidth(int width);
	void setMinimumHeight(int height);
	void setMaximumWidth(int width);
	void setMaximumHeight(int height);

	void setTexture(Texture2D* tex);
	void setRenderer(SpriteRenderer* renderer);
	void setVisible(bool visible);
	void setResizable(bool resizable);
	void setUseFixedPosition(bool fixed);
	void setUseSourceResize(bool source);
	void setSizeRatio(float ratio, bool use);			// width / height.
	void setLayoutFillPercent(int percent);
	void setClipSpace(glm::vec4 clip, bool use);
	void setColor(glm::vec4 color);

	GUIObject* getParent();
	int getChildrenNum();
	GUIObject* getChildByIndex(int index);
	glm::vec2 getPosition();
	glm::vec2 getSourcePosition();
	glm::vec2 getSize();
	glm::vec2 getSourceSize();
	glm::vec4 getClipSpace();
	glm::vec4 getColor();

	glm::vec2 getMaximumSize();
	glm::vec2 getMinimumSize();
	int getMaximumWidth();
	int getMaximumHeight();
	int getMinimumWidth();
	int getMinimumHeight();

	bool isVisible();
	bool getResizable();
	bool isUseFixedPosition();
	bool isUseSourceResize();
	bool isUseSizeRatio();
	bool isUseClipSpace();

	float getSizeRatio();
	int getLayoutFillPercent();

protected:
	glm::vec2 Position, SourcePosition;
	glm::vec2 Size, SourceSize;
	glm::vec2 minimumSize, maximumSize;
	glm::vec4 clipSpace;				// used in percentage displacement from (left, top, right, bottom).
	glm::vec4 color = glm::vec4(1.0f);

	bool visible = true;
	bool resizable = false;
	bool fixedPosition = false;
	bool useSourceResize = true;
	bool useSizeRatio = false;
	bool useClipSpace = false;

	float sizeRatio = 1.0;
	int layoutFillPercent = 1;

	Texture2D* Texture = NULL;
	SpriteRenderer* renderer = NULL;

	ISoundEngine* pSoundEngine = NULL;
	std::map<std::string, std::string> soundNames;

	GUIObject* parent = NULL;
	std::vector<GUIObject*> children;
};