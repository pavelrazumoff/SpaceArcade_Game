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
*/

#include "../Shaders/Shader.h"
#include "Texture.h"

class SpriteRenderer
{
public:
	SpriteRenderer() {}
	// Constructor (inits shaders/shapes)
	SpriteRenderer(Shader shader, int screenWidth, int screenHeight);
	// Destructor
	~SpriteRenderer();

	void init(Shader shader, int screenWidth, int screenHeight);
	void resize(int screenWidth, int screenHeight);
	// Renders a defined quad textured with given sprite
	void DrawSprite(Texture2D* texture, glm::vec2 position, glm::vec2 size, glm::vec4 color = glm::vec4(1.0f), GLfloat rotate = 0.0f, int frame = 0);
	void DrawSprite(Texture2D* texture, glm::mat4 model, glm::vec4 color = glm::vec4(1.0f), int frame = 0);

	void setUseInstanced(bool useInstanced, int amount);
	void dropInstanced();

	GLuint getVAO();
	glm::vec2 getPrevScreenDimensions();
	glm::vec2 getCurrentScreenDimensions();
	glm::vec2 getInitialScreenDimensions();
	glm::vec2 getScreenRatio();

	Shader* getShader();

private:
	// Render state
	Shader shader;
	GLuint quadVAO;
	int screenWidth, screenHeight;
	int prevScreenWidth, prevScreenHeight;
	int initialScreenWidth, initialScreenHeight;
	glm::vec2 screenRatio;

	bool useInstanced = false;
	int amountInstances = 0;

	// Initializes and configures the quad's buffer and vertex attributes
	void initRenderData();
};