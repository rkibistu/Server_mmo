#include <glad/glad.h>

#include "Renderer.h"

#include <components/SceneObject.h>
#include <components/Shader.h>
#include <components/Camera.h>
#include <components/Mesh.h>
#include <utils/Math.h>
#include <core/SceneManager.h>
#include <core/ResourceManager.h>

void Renderer::Draw(SceneObject* object) {

	// Draw children
	//for (auto it = object->_children.begin(); it != object->_children.end(); it++) {
	//	it->second->Draw();
	//}

	if (object->_meshes.empty())
		return;
	if (!object->_shader)
		return;

	object->_shader->Bind();

	rml::Matrix model = object->GetModelMatrix();
	SetMvpUniforms(object->_shader, model);

	SetLightUniforms(object->_shader);

	object->SetCustomUniforms();

	// Draw every mesh of the object
	Material* currentMaterial = nullptr;
	for (unsigned int i = 0; i < object->_meshes.size(); i++) {
		
		currentMaterial = (object->_materials.size() > i) ? object->_materials[i] : nullptr;
		DrawMesh(object->_shader, object->_meshes[i], currentMaterial, object->_drawMode);
	}

	object->_shader->Unbind();
}

void Renderer::DrawBox(rml::Vector3 min, rml::Vector3 max, rml::Vector3 color, int drawMode, std::string meshId, std::string shaderId, rml::Vector3 rotation) {

	Mesh* boxMesh = ResourceManager::GetInstance().GetMesh(meshId);
	Shader* shader = ResourceManager::GetInstance().GetShader(shaderId);
	Material* material = ResourceManager::GetInstance().GetMaterial("default");

	shader->Bind();

	rml::Vector3 boxCenter = (min + max) * 0.5;
	rml::Vector3 boxSize = (max - min) * 0.5;
	rml::Matrix positionMat;
	positionMat.SetTranslation(boxCenter);
	rml::Matrix rotationMatOY;
	rotationMatOY.SetRotationY(rotation.y);
	rml::Matrix rotationMatOX;
	rotationMatOX.SetRotationX(rotation.x);
	rml::Matrix rotationMatOZ;
	rotationMatOZ.SetRotationZ(rotation.z);
	rml::Matrix scaleMat;
	scaleMat.SetScale(boxSize);

	rml::Matrix model = scaleMat * rotationMatOX * rotationMatOZ * rotationMatOY * positionMat;
	SetMvpUniforms(shader, model);
	
	shader->SetUniform3f("u_color", color);

	DrawMesh(shader, boxMesh, material, drawMode);
}

void Renderer::DrawLine(rml::Vector3 start, rml::Vector3 end, rml::Vector3 color) {
	MeshResource* meshResource = new MeshResource();
	Vertex temp;
	temp.pos = start;
	meshResource->Vertices.push_back(temp);
	temp.pos = end;
	meshResource->Vertices.push_back(temp);
	meshResource->Indices.push_back(0);
	meshResource->Indices.push_back(1);


	Mesh* mesh = new Mesh(meshResource);
	mesh->Load();
	Shader* shader = ResourceManager::GetInstance().GetShader("color");
	Material* material = ResourceManager::GetInstance().GetMaterial("default");

	shader->Bind();

	rml::Matrix model;
	model.SetIdentity();

	SetMvpUniforms(shader, model);

	shader->SetUniform3f("u_color", color);

	DrawMesh(shader, mesh, material, GL_LINES);

	delete mesh;
}

void Renderer::DrawSphere(rml::Vector3 center, rml::Vector3 radius, rml::Vector3 color) {
	throw("Not implemented yet");
}

void Renderer::SetMvpUniforms(Shader* shader, rml::Matrix& model) {
	rml::Matrix view, projection, mvp;

	mvp = model;
	view.SetIdentity();
	projection.SetIdentity();

	Camera* activeCamera = SceneManager::GetInstance().GetActivecamera();
	if (activeCamera) {

		view = activeCamera->GetViewmatrix();
		projection = activeCamera->GetProjectionMatrix();
		mvp = model * view * projection;
	}

	shader->SetUniformMatrix4fv("Model", model);
	shader->SetUniformMatrix4fv("View", view);
	shader->SetUniformMatrix4fv("Projection", projection);
	shader->SetUniformMatrix4fv("u_mvp", mvp);
}

void Renderer::SetLightUniforms(Shader* shader) {
	//directionalLight
	shader->SetUniform3f("directional_light_direction", SceneManager::GetInstance().DirectionalLightDirection);
	shader->SetUniform3f("directional_light_color", SceneManager::GetInstance().DirectionalLightColor);

	Camera* activeCamera = SceneManager::GetInstance().GetActivecamera();
	if (activeCamera)
		shader->SetUniform3f("eye_position", activeCamera->GetPosition());

	// Point lights
	std::unordered_map<unsigned int, LightObject*> lightObjects = SceneManager::GetInstance().GetLightObjects();
	// 10 here because this is the limit in shader
	int lightsCount = lightObjects.size() < 10 ? lightObjects.size() : 10;
	shader->SetUniform1f("point_lights_count", lightsCount);
	// Set position and color
	int index = 0;
	for (auto it = lightObjects.begin(); it != lightObjects.end(); it++) {
		std::string uniformNamePosition = "point_light_positions[" + std::to_string(index) + "]";
		std::string uniformNameColor = "point_light_colors[" + std::to_string(index) + "]";
		shader->SetUniform3f(uniformNamePosition, it->second->GetPosition());
		shader->SetUniform3f(uniformNameColor, it->second->GetColor());
		index++;
	}

	// Spot lights
	shader->SetUniform1f("spot_lights_count", 0);
}

void Renderer::DrawMesh(Shader* shader, Mesh* mesh, Material* material, int drawMode) {

	if (!mesh)
		return;

	mesh->BindFilled();

	if (material) {
		if (material->DiffuseTexture) {
			material->DiffuseTexture->Bind(0);
			shader->SetUniform1i("u_Texture", 0);
		}

		shader->SetUniform3f("material_ka", material->AmbientColor);
		shader->SetUniform3f("material_kd", material->DiffuseColor);
		shader->SetUniform3f("material_ks", material->SpecularColor);
		shader->SetUniform1i("material_shininess", material->Shininess);
	}
	mesh->Draw(drawMode);

	mesh->Unbind();
	if (material && material->DiffuseTexture)
		material->DiffuseTexture->Unbind();
}
