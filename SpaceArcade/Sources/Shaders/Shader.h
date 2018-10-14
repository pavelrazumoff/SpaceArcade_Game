#pragma once
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