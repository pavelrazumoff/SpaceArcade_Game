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

class Shader
{
public:
	// constructor reads and builds the shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);

	void load(const char* vertexPath, const char* fragmentPath);
	void loadGeometryShader(const char* geometryPath);
	void loadSingleShader(const char* shaderPath, std::string shaderType);
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec3(const std::string &name, float x, float y, float z);
	void setVec3(const std::string &name, glm::vec3 value);
	void setVec4(const std::string &name, glm::vec4 value);
	void setMat4(const std::string &name, glm::mat4 value);

	void setUseOnlyDepth(bool useDepth);
	unsigned int getShaderProgram();
	bool getUseOnlyDepth();

private:
	unsigned int ShaderProgram;
	bool useOnlyDepth = false;
};