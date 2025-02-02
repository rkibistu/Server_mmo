#pragma once

#include <utils/Math.h>

class SceneObject;
class Mesh;
class Shader;
struct Material;

class Renderer {
public:
	static void Draw(SceneObject* object);
	static void DrawBox(rml::Vector3 min, rml::Vector3 max, rml::Vector3 color, int drawMode,std::string meshId = "1", std::string shaderId = "color", rml::Vector3 rotation = rml::Vector3(0, 0, 0));
	static void DrawLine(rml::Vector3 start, rml::Vector3 end, rml::Vector3 color);
	static void DrawSphere(rml::Vector3 center, rml::Vector3 radius, rml::Vector3 color);

private:
	static void SetMvpUniforms(Shader* shader, rml::Matrix& model);
	static void SetLightUniforms(Shader* shader);
	static void DrawMesh(Shader* shader, Mesh* mesh, Material* material, int drawMode);

private:
	//Mesh* _lineMesh = nullptr;
	//Mesh* _boxMesh = nullptr;
	//Mesh* _sphereMesh = nullptr;
};
