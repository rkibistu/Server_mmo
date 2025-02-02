#pragma once

#include <string>

#include <utils/Math.h>

/**
 * Structure that represents the data found in resources.xml. This data is used to find
 * the shaders filepaths.
 */
struct ShaderResource {

	std::string ID;

	std::string Path;
	std::string VertexShaderFilename;
	std::string FragmentShaderFilename;

	std::string VertexFilePath() { return Path + VertexShaderFilename; }
	std::string FragmentFilePath() { return Path + FragmentShaderFilename; }
};

/**
 * it is a RESOURCE that represents a shader program.
 * Used to manage opengl shader.
 */
class Shader {
	friend class ResourceManager;

private:
	// Private constructors so only ResourceManager can create resources 
	Shader() = delete;
	Shader(ShaderResource* shaderResource);
	Shader(const Shader&) = default;
	Shader& operator=(const Shader&) = default;
public:
	~Shader();

	int Load();

	void Bind();
	void Unbind();

	bool IsLoaded() { return _loaded; }

	inline std::string GetID() { return _shaderResource->ID; }

	//Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform2f(const std::string& name, rml::Vector2 v);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, rml::Vector3 v);
	void SetUniformMatrix4fv(const std::string& name, const rml::Matrix& matrix);
private:
	//Read files and compile shaders
	int Init(char* fileVertexShader, char* fileFragmentShader);

	int GetUniformLocation(const std::string& name);

private:
	bool _loaded = false;
	ShaderResource* _shaderResource = nullptr;
	unsigned int _programId;

	/** Used for debug */
	int  success;
	char infoLog[512];
};
