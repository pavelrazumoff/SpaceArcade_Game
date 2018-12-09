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

#include "../../Header.h"
#include "../Shaders/Shader.h"
#include "Texture.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager
{
public:
	ResourceManager() { }
	~ResourceManager();

	void init();

	// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	Shader LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, std::string name);
	// Loads (and generates) a texture from file
	Texture2D* LoadTexture(const GLchar *file, std::string name, bool gamma, TextureParams* texParams = NULL);
	Texture2D* LoadCubemap(std::vector<const GLchar*> faces, std::string name, bool gamma, TextureParams* texParams = NULL);
	//Loads font.
	void loadFont(std::string fontPath, std::string fontType, int fontSize);
	// Adds sound path to the map.
	void addSound(std::string soundPath, std::string soundName);

	// Retrieves a stored sader
	Shader GetShader(std::string name);
	// Retrieves a stored texture
	Texture2D* GetTexture(std::string name);
	Texture2D* GetCubemap(std::string name);

	ISoundEngine* getSoundEngine();
	std::string getSoundPath(std::string name);
	// Properly de-allocates all loaded resources
	void clear();

	// Resource storage
	std::map<std::string, Shader>    Shaders;
	std::map<std::string, Texture2D*> Textures;
	std::map<std::string, Texture2D*> Cubemaps;
	std::map<std::string, Font> Fonts;
	std::map<std::string, std::string> sounds;

private:
	FT_Library ft;
	ISoundEngine *SoundEngine = NULL;
};