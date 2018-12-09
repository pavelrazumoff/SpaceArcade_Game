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

#include "Shader.h"

Shader::Shader()
{
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	load(vertexPath, fragmentPath);
}

void Shader::load(const char* vertexPath, const char* fragmentPath)
{
	this->ShaderProgram = glCreateProgram();
	loadSingleShader(vertexPath, "vertex");
	loadSingleShader(fragmentPath, "fragment");
}

void Shader::loadGeometryShader(const char* geometryPath)
{
	loadSingleShader(geometryPath, "geometry");
}

void Shader::loadSingleShader(const char* shaderPath, std::string shaderType)
{
	// 1. retrieve the shader source code from filePath.
	std::string shaderCode;
	std::ifstream shaderFile;
	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		// read file’s buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* ShaderCode = shaderCode.c_str();

	// 2. compile shaders
	unsigned int shaderId;
	int success;
	char infoLog[512];

	if (!shaderType.compare("vertex"))
		shaderId = glCreateShader(GL_VERTEX_SHADER);
	else
		if (!shaderType.compare("fragment"))
			shaderId = glCreateShader(GL_FRAGMENT_SHADER);
		else
			if (!shaderType.compare("geometry"))
				shaderId = glCreateShader(GL_GEOMETRY_SHADER);

	glShaderSource(shaderId, 1, &ShaderCode, NULL);
	glCompileShader(shaderId);
	// print compile errors if any
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// shader Program
	glAttachShader(this->ShaderProgram, shaderId);
	glLinkProgram(this->ShaderProgram);

	// print linking errors if any
	glGetProgramiv(this->ShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->ShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// delete the shaders as they’re linked into our program now and no longer necessery
	glDeleteShader(shaderId);
}

void Shader::use()
{
	glUseProgram(ShaderProgram);
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ShaderProgram, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ShaderProgram, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ShaderProgram, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(ShaderProgram, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3f(glGetUniformLocation(ShaderProgram, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4f(glGetUniformLocation(ShaderProgram, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string &name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setUseOnlyDepth(bool useDepth)
{
	useOnlyDepth = useDepth;
}

bool Shader::getUseOnlyDepth()
{
	return useOnlyDepth;
}

unsigned int Shader::getShaderProgram()
{
	return ShaderProgram;
}