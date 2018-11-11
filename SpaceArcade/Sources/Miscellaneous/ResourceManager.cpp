#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{
	clear();
}

void ResourceManager::init()
{
	// Initialize FreeType library.
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Initialize irrKlang Sound library.
	SoundEngine = createIrrKlangDevice();
}

Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, std::string name)
{
	Shader shader;
	shader.load(vShaderFile, fShaderFile);
	Shaders[name] = shader;
	return Shaders[name];
}

Texture2D* ResourceManager::LoadTexture(const GLchar *file, std::string name, bool gamma, TextureParams* texParams)
{
	Texture2D* texture = new Texture2D();
	if (texParams)
		texture->setTextureParams(texParams);

	texture->GenerateTexture(file, gamma);
	Textures[name] = texture;
	return Textures[name];
}

Texture2D* ResourceManager::LoadCubemap(std::vector<const GLchar*> faces, std::string name, bool gamma, TextureParams* texParams)
{
	Texture2D* cubemap = new Texture2D();
	if (texParams)
		cubemap->setTextureParams(texParams);

	cubemap->Filter_Min = GL_LINEAR;
	cubemap->GenerateCubemap(faces, gamma);
	Cubemaps[name] = cubemap;
	return Cubemaps[name];
}

void ResourceManager::loadFont(std::string fontPath, std::string fontType, int fontSize)
{
	// Load specific font.
	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set font size.
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	std::map<GLchar, Character> Characters;

	// Load font for 128 characters.
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
			GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	Fonts[fontType] = Characters;

	FT_Done_Face(face);
}

void ResourceManager::addSound(std::string soundPath, std::string soundName)
{
	sounds[soundName] = soundPath;
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Texture2D* ResourceManager::GetTexture(std::string name)
{
	auto it = Textures.find(name);
	if (it != Textures.end())
		return Textures[name];
	return NULL;
}

Texture2D* ResourceManager::GetCubemap(std::string name)
{
	return Cubemaps[name];
}

ISoundEngine* ResourceManager::getSoundEngine()
{
	return SoundEngine;
}

std::string ResourceManager::getSoundPath(std::string name)
{
	auto it = sounds.find(name);
	if (it != sounds.end())
		return it->second;
	return "";
}

void ResourceManager::clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.getShaderProgram());
	// (Properly) delete all textures
	for (auto iter : Textures)
	{
		glDeleteTextures(1, &iter.second->ID);
		delete iter.second;
	}
	for (auto iter : Cubemaps)
	{
		glDeleteTextures(1, &iter.second->ID);
		delete iter.second;
	}

	FT_Done_FreeType(ft);

	SoundEngine->drop();
}