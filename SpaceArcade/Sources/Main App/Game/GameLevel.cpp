#include "GameLevel.h"

void GameLevel::init(Texture2D cubemap, Shader cubemapShader)
{
	this->backgroundCubemap = cubemap;
	this->cubemapShader = cubemapShader;
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
	glm::vec2 initialScreen = renderer.getInitialScreenDimensions();
	glm::vec2 currentScreen = renderer.getCurrentScreenDimensions();

	glm::vec2 screenRatio = glm::vec2(currentScreen.x / initialScreen.x, currentScreen.y / initialScreen.y);

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

	spacecraft.Draw(renderer, screenRatio);
}

void GameLevel::handleInput(GLFWwindow *window, float delta)
{
	spacecraft.handleInput(window, delta);
}

GLboolean GameLevel::IsCompleted()
{
	return GL_FALSE;
}