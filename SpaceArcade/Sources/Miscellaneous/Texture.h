#pragma once
#include "../../Header.h"

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
struct TextureParams
{
	GLuint Wrap_S = GL_CLAMP_TO_EDGE; // Wrapping mode on S axis
	GLuint Wrap_T = GL_CLAMP_TO_EDGE; // Wrapping mode on T axis
	GLuint Wrap_R = GL_CLAMP_TO_EDGE; // Wrapping mode on R axis
	GLuint Filter_Min = GL_LINEAR_MIPMAP_LINEAR; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Max = GL_LINEAR; // Filtering mode if texture pixels > screen pixels
};

class Texture2D
{
public:
	// Constructor (sets default texture modes)
	Texture2D();
	// Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
	GLuint ID;
	// Texture image dimensions
	GLuint Width, Height; // Width and height of loaded image in pixels
	// Texture Format
	GLuint Internal_Format; // Format of texture object
	GLuint Image_Format; // Format of loaded image
	// Texture configuration
	GLuint Wrap_S; // Wrapping mode on S axis
	GLuint Wrap_T; // Wrapping mode on T axis
	GLuint Wrap_R; // Wrapping mode on R axis
	GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels
	// Generates texture from image data
	void GenerateTexture(const GLchar *file, bool gamma);
	void GenerateCubemap(std::vector<const GLchar*> faces, bool gamma);

	void setTextureParams(TextureParams* params);

	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void BindTexture() const;
	void BindCubemap() const;

	int numOfColumns = 1;
	int numOfRows = 1;
	int numOfFrames = 1;
};