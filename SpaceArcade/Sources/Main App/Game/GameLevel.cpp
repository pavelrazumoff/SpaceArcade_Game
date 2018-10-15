#include "GameLevel.h"

GameLevel::~GameLevel()
{
	clear();
}

void GameLevel::init(Texture2D cubemap, Shader cubemapShader, SpriteRenderer* renderer)
{
	this->backgroundCubemap = cubemap;
	this->cubemapShader = cubemapShader;
	this->renderer = renderer;

	spacecraft = new SpacecraftObject();
}

void GameLevel::update(float delta)
{
	spacecraft->update(delta);
}

void GameLevel::Draw()
{
	// draw skybox as last.
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content.

	cubemapShader.use();

	glm::mat4 modelMat = glm::mat4();
	modelMat = glm::rotate(modelMat, -(float)glfwGetTime() / 10, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::rotate(modelMat, -(float)glfwGetTime() / 100, glm::vec3(0.0f, 1.0f, 0.0f));

	cubemapShader.setMat4("model", modelMat);
	glActiveTexture(GL_TEXTURE0);
	backgroundCubemap.BindCubemap();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.

	glDisable(GL_DEPTH_TEST);
	spacecraft->Draw();
	glEnable(GL_DEPTH_TEST);
}

void GameLevel::resize()
{
	spacecraft->resize();
}

void GameLevel::handleInput(GLFWwindow *window, float delta)
{
	spacecraft->handleInput(window, delta);
}

void GameLevel::processKey(int key, int action, bool* key_pressed)
{
	spacecraft->processKey(key, action, key_pressed);
}

void GameLevel::setScreenIndents(glm::vec4 indents)
{
	screenIndents = indents;
}

glm::vec4 GameLevel::getScreenIndents()
{
	return screenIndents;
}

SpriteRenderer* GameLevel::getRenderer()
{
	return renderer;
}

GLboolean GameLevel::IsCompleted()
{
	return GL_FALSE;
}

void GameLevel::clear()
{
	if (spacecraft)
	{
		delete spacecraft;
		spacecraft = NULL;
	}
}