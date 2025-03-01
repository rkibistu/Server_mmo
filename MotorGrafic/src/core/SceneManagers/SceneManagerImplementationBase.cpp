#include "SceneManagerImplementationBase.h"

#include <components/SceneObject.h>
#include <core/ResourceManager.h>
#include <core/SceneXmlParser.h>
#include <components/Model.h>
#include <components/SceneObjects/iSceneObject.h>
#include <components/Camera.h>
#include <components/LightObject.h>

unsigned int SceneManagerImplementationBase::_sceneObjectIdCounter = 0;

std::vector<iSceneObject*> SceneManagerImplementationBase::GetAllObjects() {

	std::vector<iSceneObject*> result;
	for (auto it = _sceneObjects.begin(); it != _sceneObjects.end(); it++) {

		std::vector<iSceneObject*> objects = it->second->GetAllObjects();
		result.insert(result.end(), objects.begin(), objects.end());
	}

	return result;
}

SceneManagerImplementationBase::~SceneManagerImplementationBase() {

	for (auto it = _sceneObjects.begin(); it != _sceneObjects.end(); it++) {
		if (it->second)
			delete it->second;
	}
}


iSceneObject* SceneManagerImplementationBase::CreateAndAddSceneObject(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) {
	iSceneObject* sceneObject = CreateUsingModel(name, modelId, shaderId, position, rotation, scale, movementType, parent);
	if (parent == nullptr)
		AddSceneObject(sceneObject);

	return sceneObject;
}

iSceneObject* SceneManagerImplementationBase::CreateUsingModel(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) {
	unsigned int id = _sceneObjectIdCounter++;
	SceneObject* sceneObject = new SceneObject(id, name, movementType);

	Shader* shader = ResourceManager::GetInstance().GetShader(shaderId);
	if (!shader) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using shaderId with value: " << shaderId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	sceneObject->SetShader(shader);

	Model* model = ResourceManager::GetInstance().GetModel(modelId);
	if (!model) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using modelId with value: " << modelId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	ModelNode* rootModelNode = model->GetRootNode();
	for (unsigned int i = 0; i < rootModelNode->MeshMaterialIndices.size(); i++) {
		sceneObject->AddMesh(model->GetMesh(rootModelNode->MeshMaterialIndices[i]));
		sceneObject->AddMaterial(model->GetMaterial(rootModelNode->MeshMaterialIndices[i]));
	}
	for (unsigned int i = 0; i < rootModelNode->Children.size(); i++) {
		CreateSceneObjectChild(model, rootModelNode->Children[i], sceneObject);
	}


	sceneObject->SetPosition(position);
	sceneObject->SetRotation(rotation);
	sceneObject->SetScale(scale);

	sceneObject->SetParent(parent);

	return sceneObject;
}

iSceneObject* SceneManagerImplementationBase::CreateUsingMesh(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) {
	unsigned int id = _sceneObjectIdCounter++;
	SceneObject* obj = new SceneObject(id, name, movementType);

	Shader* shader = ResourceManager::GetInstance().GetShader(shaderId);
	if (!shader) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using shaderId with value: " << shaderId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	obj->SetShader(shader);

	Mesh* mesh = ResourceManager::GetInstance().GetMesh(meshId);
	if (!mesh) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using meshId with value: " << meshId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	obj->AddMesh(mesh);

	obj->SetPosition(position);
	obj->SetRotation(rotation);
	obj->SetScale(scale);

	obj->SetParent(parent);

	return obj;
}

LightObject* SceneManagerImplementationBase::CreateAndAddLightObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, rml::Vector3 color, ObjectMovementType movementType, iSceneObject* parent) {
	unsigned int id = _sceneObjectIdCounter++;
	LightObject* lightObject = new LightObject(id, name, color);

	Shader* shader = ResourceManager::GetInstance().GetShader(shaderId);
	if (!shader) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using shaderId with value: " << shaderId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	lightObject->SetShader(shader);

	Mesh* mesh = ResourceManager::GetInstance().GetMesh(meshId);
	if (!mesh) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using meshId with value: " << meshId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	lightObject->AddMesh(mesh);

	lightObject->SetPosition(position);
	lightObject->SetRotation(rotation);
	lightObject->SetScale(scale);

	lightObject->SetParent(parent);

	if (parent == nullptr)
		AddLightObject(lightObject);

	return lightObject;
}

void SceneManagerImplementationBase::RemoveObject(unsigned int id) {
	auto it = _sceneObjects.find(id);
	if (it != _sceneObjects.end()) {
		delete it->second;
		_sceneObjects.erase(it);
	}

	auto itLight = _lightObjects.find(id);
	if (itLight != _lightObjects.end()) {
		//already deleted
		_lightObjects.erase(itLight);
	}
}

void SceneManagerImplementationBase::AddSceneObject(iSceneObject* obj) {
	assert(_sceneObjects.find(obj->GetID()) == _sceneObjects.end());
	assert(obj->GetParent() == nullptr);

	_sceneObjects[obj->GetID()] = obj;
}

void SceneManagerImplementationBase::AddLightObject(LightObject* obj) {
	assert(_lightObjects.find(obj->GetID()) == _lightObjects.end());
	AddSceneObject(obj);

	_lightObjects[obj->GetID()] = obj;
}

void SceneManagerImplementationBase::CreateSceneObjectChild(Model* model, ModelNode* modelNode, iSceneObject* parent) {
	unsigned int id = _sceneObjectIdCounter++;
	SceneObject* sceneObject = new SceneObject(id, parent->GetName() + "_" + std::to_string(id), SEMI_STATIC);

	for (unsigned int i = 0; i < modelNode->MeshMaterialIndices.size(); i++) {
		sceneObject->AddMesh(model->GetMesh(modelNode->MeshMaterialIndices[i]));
		sceneObject->AddMaterial(model->GetMaterial(modelNode->MeshMaterialIndices[i]));
	}
	for (unsigned int i = 0; i < modelNode->Children.size(); i++) {
		CreateSceneObjectChild(model, modelNode->Children[i], sceneObject);
	}

	sceneObject->SetShader(parent->GetShader());

	sceneObject->SetParent(parent);
}
