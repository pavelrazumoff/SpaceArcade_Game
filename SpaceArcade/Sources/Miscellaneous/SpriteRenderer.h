#pragma once
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
	void DrawSprite(Texture2D* texture, glm::vec2 position, glm::vec2 size, glm::vec4 color = glm::vec4(0.0f), GLfloat rotate = 0.0f, int frame = 0);
	void DrawSprite(Texture2D* texture, glm::mat4 model, glm::vec4 color = glm::vec4(0.0f), int frame = 0);

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