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

#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(Shader shader, int screenWidth, int screenHeight)
{
	init(shader, screenWidth, screenHeight);
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::init(Shader shader, int screenWidth, int screenHeight)
{
	this->shader = shader;
	this->prevScreenWidth = screenWidth;
	this->prevScreenHeight = screenHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->initialScreenWidth = screenWidth;
	this->initialScreenHeight = screenHeight;

	screenRatio = glm::vec2(1.0f, 1.0f);

	this->initRenderData();
}

void SpriteRenderer::resize(int screenWidth, int screenHeight)
{
	this->prevScreenWidth = this->screenWidth;
	this->prevScreenHeight = this->screenHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	screenRatio = glm::vec2((float)screenWidth / (float)prevScreenWidth, (float)screenHeight / (float)prevScreenHeight);
}

glm::vec2 SpriteRenderer::getPrevScreenDimensions()
{
	return glm::vec2(prevScreenWidth, prevScreenHeight);
}

glm::vec2 SpriteRenderer::getCurrentScreenDimensions()
{
	return glm::vec2(screenWidth, screenHeight);
}

glm::vec2 SpriteRenderer::getInitialScreenDimensions()
{
	return glm::vec2(initialScreenWidth, initialScreenHeight);
}

glm::vec2 SpriteRenderer::getScreenRatio()
{
	return screenRatio;
}

void SpriteRenderer::DrawSprite(Texture2D* texture, glm::vec2 position, glm::vec2 size, glm::vec4 color, GLfloat rotate, int frame)
{
	// Prepare transformations
	this->shader.use();
	
	GLint prevBlendFunc;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &prevBlendFunc);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->screenWidth), static_cast<GLfloat>(this->screenHeight), 0.0f, -1.0f, 1.0f);

	if(texture)
		this->shader.setBool("useImage", true);
	else
		this->shader.setBool("useImage", false);

	this->shader.setInt("image", 0);
	this->shader.setMat4("projection", projection);
	this->shader.setVec4("background_color", color);

	this->shader.setInt("frame", frame);

	if (texture)
	{
		this->shader.setInt("numOfColumns", texture->numOfColumns);
		this->shader.setInt("numOfRows", texture->numOfRows);
	}

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	this->shader.setMat4("model", model);

	this->shader.setBool("useInstances", false);

	if (texture)
	{
		glActiveTexture(GL_TEXTURE0);
		texture->BindTexture();
	}

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, prevBlendFunc);
	glDisable(GL_BLEND);
}

void SpriteRenderer::DrawSprite(Texture2D* texture, glm::mat4 model, glm::vec4 color, int frame)
{
	// Prepare transformations
	this->shader.use();

	GLint prevBlendFunc;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &prevBlendFunc);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->screenWidth), static_cast<GLfloat>(this->screenHeight), 0.0f, -1.0f, 1.0f);

	if (texture)
		this->shader.setBool("useImage", true);
	else
		this->shader.setBool("useImage", false);

	this->shader.setInt("image", 0);
	this->shader.setMat4("projection", projection);
	this->shader.setVec4("background_color", color);

	this->shader.setInt("frame", frame);

	if (texture)
	{
		this->shader.setInt("numOfColumns", texture->numOfColumns);
		this->shader.setInt("numOfRows", texture->numOfRows);
	}

	this->shader.setMat4("model", model);

	this->shader.setBool("useInstances", useInstanced);

	if (texture)
	{
		glActiveTexture(GL_TEXTURE0);
		texture->BindTexture();
	}

	glBindVertexArray(this->quadVAO);

	if (useInstanced)
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amountInstances);
	else
		glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, prevBlendFunc);
	glDisable(GL_BLEND);
}

void SpriteRenderer::initRenderData()
{
	// Configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::setUseInstanced(bool useInstanced, int amount)
{
	this->useInstanced = useInstanced;
	this->amountInstances = amount;
}

void SpriteRenderer::dropInstanced()
{
	this->useInstanced = false;
	this->amountInstances = 0;
}

Shader* SpriteRenderer::getShader()
{
	return &shader;
}

GLuint SpriteRenderer::getVAO()
{
	return quadVAO;
}