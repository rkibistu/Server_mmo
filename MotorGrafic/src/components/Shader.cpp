#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <utils/Utils.h>
#include <core/Defines.h>
#include "Shader.h"

#include <core/MemoryDebug.h>

Shader::Shader(ShaderResource* shaderResource)
	: _shaderResource(shaderResource) {
}

Shader::~Shader() {
	glDeleteProgram(_programId);

	if (_shaderResource)
		delete _shaderResource;
}

int Shader::Init(char* fileVertexShader, char* fileFragmentShader) {
	int ret = MY_SUCCES_CODE;
	std::string vertexShaderSourceStr = Utils::readFile(fileVertexShader);
	if (vertexShaderSourceStr == "")
		return MY_ERROR_CODE;
	std::string fragmentShaderSourceStr = Utils::readFile(fileFragmentShader);
	if (fragmentShaderSourceStr == "")
		return MY_ERROR_CODE;
	const char* vertexShaderSource = vertexShaderSourceStr.c_str();
	const char* fragmentShaderSource = fragmentShaderSourceStr.c_str();

	//Vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//Check for shader errors at compilation
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		ret = MY_ERROR_CODE;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		ret = MY_ERROR_CODE;
	}

	//Shader program
	_programId = glCreateProgram();
	glAttachShader(_programId, vertexShader);
	glAttachShader(_programId, fragmentShader);
	glLinkProgram(_programId);
	// Check errors 
	glGetProgramiv(_programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(_programId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		ret = MY_ERROR_CODE;
	}

	//Cleanup
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return ret;
}

int Shader::Load() {

	int ret = Init(
		(char*)_shaderResource->VertexFilePath().c_str(),
		(char*)_shaderResource->FragmentFilePath().c_str()
	);

	if (ret != MY_SUCCES_CODE) return ret;

	_loaded = true;
	return MY_SUCCES_CODE;
}

void Shader::Bind() {
	if (!_loaded) {
		std::cout << "[SHADER][ERROR]: Can't bind a shader that is not loaded. Call Load method before. ShaderID: " << _shaderResource->ID << "\n";
		return;
	}
	glUseProgram(_programId);
}

void Shader::Unbind() {
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value) {

	glUniform1i(GetUniformLocation(name), value);
}
void Shader::SetUniform1f(const std::string& name, float value) {

	glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniform2f(const std::string& name, float v0, float v1) {

	glUniform2f(GetUniformLocation(name), v0, v1);
}
void Shader::SetUniform2f(const std::string& name, rml::Vector2 v) {

	glUniform2f(GetUniformLocation(name), v.x, v.y);
}
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {

	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}
void Shader::SetUniform3f(const std::string& name, rml::Vector3 v) {

	glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
}

void Shader::SetUniformMatrix4fv(const std::string& name, const rml::Matrix& matrix) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix.m[0][0]);
}

// PRIVATE

int Shader::GetUniformLocation(const std::string& name) {

	int location = glGetUniformLocation(_programId, name.c_str());
	if (location == -1) {

		std::string generalName = _shaderResource->FragmentShaderFilename.substr(0, _shaderResource->FragmentShaderFilename.find("FS"));
		//std::cout << "[WARNING]: Uniform '" << name << "' doesn't exist on shader " << generalName << "!" << std::endl;
	}

	return location;
}

