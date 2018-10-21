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

void SpriteRenderer::DrawSprite(Texture2D* texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, int frame)
{
	// Prepare transformations
	this->shader.use();
	
	GLint prevBlendFunc;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &prevBlendFunc);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->screenWidth), static_cast<GLfloat>(this->screenHeight), 0.0f, -1.0f, 1.0f);

	this->shader.setInt("image", 0);
	this->shader.setMat4("projection", projection);

	this->shader.setInt("frame", frame);
	this->shader.setInt("numOfColumns", texture->numOfColumns);
	this->shader.setInt("numOfRows", texture->numOfRows);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	this->shader.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	texture->BindTexture();

	glBindVertexArray(this->quadVAO);
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