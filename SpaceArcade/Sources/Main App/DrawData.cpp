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

#include "MainApp.h"

void MainApp::render()
{
	// Draw scene as normal.
	// first render scene in specific framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we’re not using the stencil buffer now.

	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix.
	glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 5000.0f);

	// Store uniform buffer data.
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// skybox cube
	glBindVertexArray(skyboxVAO);

	switch (currentPage)
	{
	case PageType::MainMenu:
	case PageType::Settings:
	case PageType::Credits:
		drawMenuBackground();
		break;
	case PageType::Game:
	case PageType::PauseGame:
	case PageType::GameOver:
	case PageType::StationDialogue:
		drawScene();
		break;
	default:
		break;
	}

	// second render framebuffer as texture for post-processing.
	// blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Draw intermediate's screenTexture to the bloom framebuffer as normal.
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO); // back to default
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Here we have to render to final fbo with two outputs.
	res_manager->Shaders["Final"].use();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);

	res_manager->Shaders["Final"].setInt("screenTexture", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Blur here.
	// 2. blur bright fragments with two-pass Gaussian Blur 
	// --------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool horizontal = true, first_iteration = true;

	// If we use bloom, blur rendered scene, if not pass further.
	if (useBloom)
	{
		unsigned int amount = 10;
		res_manager->Shaders["Blur"].use();

		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			res_manager->Shaders["Blur"].setInt("horizontal", horizontal);
			res_manager->Shaders["Blur"].setInt("image", 0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? finalTextures[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
	}

	// Bind final source framebuffer and render finalTextures[0] for final post-processing.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	res_manager->Shaders["Screen"].use();
	res_manager->Shaders["Screen"].setBool("gammaCorrection", useGammaCorrection);
	res_manager->Shaders["Screen"].setBool("useHDR", useHDR);
	res_manager->Shaders["Screen"].setInt("useBloom", useBloom);
	res_manager->Shaders["Screen"].setFloat("exposure", 1.0f);

	res_manager->Shaders["Screen"].setInt("screenTexture", 0);
	res_manager->Shaders["Screen"].setInt("bloomTexture", 1);
	res_manager->Shaders["Screen"].setInt("ssaoTexture", 2);

	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, finalTextures[0]);

	if (useBloom)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);

	drawPage(currentPage);
	if(currentPage == PageType::StationDialogue)
		drawPage(PageType::Game);

	glEnable(GL_DEPTH_TEST);
}

void MainApp::drawScene()
{
	pCurrentLevel->draw();
}

void MainApp::drawMenuBackground()
{
	// draw skybox as last.
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content.

	Shader cubemapShader = res_manager->GetShader("Skybox");
	Texture2D* backgroundCubemap = res_manager->GetCubemap("BlueSpace");
	cubemapShader.use();

	glm::mat4 modelMat = glm::mat4();
	modelMat = glm::rotate(modelMat, -(float)glfwGetTime() / 100, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::rotate(modelMat, -(float)glfwGetTime() / 1000, glm::vec3(0.0f, 1.0f, 0.0f));

	cubemapShader.setMat4("model", modelMat);
	glActiveTexture(GL_TEXTURE0);
	backgroundCubemap->BindCubemap();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.
}

void MainApp::drawPage(int type)
{
	for (int i = 0; i < gui_objects[type].size(); ++i)
		gui_objects[type][i]->draw();
}

void MainApp::RenderText(std::string fontType, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state
	font_shader.use();
	glUniform3f(glGetUniformLocation(font_shader.getShaderProgram(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(fontVAO);

	font_shader.setBool("useClipSpace", false);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = res_manager->Fonts[fontType].characters[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
		{ xpos, ypos + h, 0.0, 0.0 },
		{ xpos, ypos, 0.0, 1.0 },
		{ xpos + w, ypos, 1.0, 1.0 },
		{ xpos, ypos + h, 0.0, 0.0 },
		{ xpos + w, ypos, 1.0, 1.0 },
		{ xpos + w, ypos + h, 1.0, 0.0 }
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
}