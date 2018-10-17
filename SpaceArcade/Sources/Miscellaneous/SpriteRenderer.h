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
	void DrawSprite(Texture2D* texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, int frame = 0);

	glm::vec2 getPrevScreenDimensions();
	glm::vec2 getCurrentScreenDimensions();
	glm::vec2 getInitialScreenDimensions();
	glm::vec2 getScreenRatio();

private:
	// Render state
	Shader shader;
	GLuint quadVAO;
	int screenWidth, screenHeight;
	int prevScreenWidth, prevScreenHeight;
	int initialScreenWidth, initialScreenHeight;
	glm::vec2 screenRatio;

	// Initializes and configures the quad's buffer and vertex attributes
	void initRenderData();
};