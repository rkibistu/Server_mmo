#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "SceneObject.h"
#include <core/SceneManager.h>
#include <core/Renderer.h>

#include <core/MemoryDebug.h>

SceneObject::SceneObject(unsigned int id, std::string name, ObjectMovementType movementType)
	: _id(id), _name(name), _movementType(movementType) {

	_position = rml::Vector3(0, 0, 0);
	_rotation = rml::Vector3(0, 0, 0);
	_scale = rml::Vector3(1, 1, 1);

	_drawMode = GL_TRIANGLES;

	GenerateAABB();
}

SceneObject::~SceneObject() {

	//Don't delete mesh/material/shader they are managed byu resource manager

	for (auto it = _children.begin(); it != _children.end(); it++) {
		if (it->second) {

			delete it->second;
			it->second = nullptr;
		}
	}

	if (_bounds) {
		delete _bounds;
		_bounds = nullptr;
	}
	if (_worldBounds) {
		delete _worldBounds;
		_worldBounds = nullptr;
	}
}

void SceneObject::Start() {

}

void SceneObject::Update(float deltaTime) {
	for (auto it = _children.begin(); it != _children.end(); it++) {
		it->second->Update(deltaTime);
	}
}

void SceneObject::Draw() {
	Renderer::Draw(this);

	CustomDraw();

	if (!_debug)
		return;

	AABB* aabb = GetAABB();
	if (!aabb)
		return;
	Renderer::DrawBox(
		aabb->Min,
		aabb->Max,
		rml::Vector3(0, 0, 1),
		GL_LINES
	);
}

void SceneObject::AddMesh(Mesh* mesh) {
	_meshes.push_back(mesh);

	// AABB has to contain all the meshes so we have to regenerate it
	GenerateAABB();
}

void SceneObject::SetMaterial(Material* material, unsigned int index, bool all) {

	if (all == true) {
		for (int i = 0; i < _materials.size(); i++) {
			_materials[i] = material;
		}
		return;
	}

	if (index >= _materials.size())
		return;

	if (!_materials[index])
		return;

	_materials[index] = material;
}

void SceneObject::SetParent(iSceneObject* parent) {
	if (parent)
		parent->AddChild(this);
	else _parent = nullptr;
}

void SceneObject::AddChild(iSceneObject* child) {
	
	unsigned int id = child->GetID();

	//Chech for a child with the same id. This should not happen
	if (_children.find(id) != _children.end()) {
		std::cout << "[SCENEOBJECT][ERROR]: Add new child with the same id as an old child. This should not happen! 2 objects should not have same id EVER!. ID: " << id << "\n";
		return;
	}

	// Remove child from old parent
	iSceneObject* p = child->GetParent();
	if (p) {
		p->RemoveChild(child->GetID());
	}

	// Change parent pointer for child object
	static_cast<SceneObject*>(child)->_parent = this;

	// Add child to new parent
	_children[id] = child;

	//The sceneobject vector from SceneManager handles only root objects of the hierrachy.
	//All children are updated/destroyed/etc by their parents.
	//So, if we add a child that was before a root object we want to remove it from the vector
	SceneManager::GetInstance().RemoveObject(child->GetID());
}

void SceneObject::RemoveChild(unsigned int id) {

	// Removed child will become an orphan
	//So we must: set parent pointer to null, add child to SceneManager vector as a root object and remove it from old parent 

	auto it = _children.find(id);
	if (it != _children.end()) {

		//set parent pointer to null
		it->second->SetParent(nullptr);

		//Add obejct to scenemanager vector
		SceneManager::GetInstance().AddSceneObject(it->second);

		//Remove object from parent
		_children.erase(it);
	}
}

rml::Vector3 SceneObject::GetWorldPosition() {
	if (_parent == nullptr)
		return _position;

	return _parent->GetWorldPosition() + _position;
}

// PRIVATE

rml::Matrix SceneObject::GetModelMatrix() {
	rml::Matrix positionMat;
	positionMat.SetTranslation(_position);

	rml::Matrix rotationMatOY;
	rotationMatOY.SetRotationY(_rotation.y);
	rml::Matrix rotationMatOX;
	rotationMatOX.SetRotationX(_rotation.x);
	rml::Matrix rotationMatOZ;
	rotationMatOZ.SetRotationZ(_rotation.z);

	rml::Matrix scaleMat;
	scaleMat.SetScale(_scale);

	// Get parent transformations
	rml::Matrix parentModel;
	parentModel.SetIdentity();
	if (_parent)
		parentModel = static_cast<SceneObject*>(_parent)->GetModelMatrix();

	//Apply local transformations and then apply parent ones
	return scaleMat * rotationMatOX * rotationMatOZ * rotationMatOY * positionMat * parentModel;
}

void SceneObject::GenerateAABB() {

	// Initialize min and max points to extreme values
	rml::Vector3 min(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()
	);
	rml::Vector3 max(
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest()
	);

	// Iterate over all vertices in all meshes
	for (auto it = _meshes.begin(); it != _meshes.end(); it++) {

		for (const Vertex& vertex : (*it)->GetMeshResource()->Vertices) {
			// Update min and max bounds based on vertex positions
			min.x = std::min(min.x, vertex.pos.x);
			min.y = std::min(min.y, vertex.pos.y);
			min.z = std::min(min.z, vertex.pos.z);

			max.x = std::max(max.x, vertex.pos.x);
			max.y = std::max(max.y, vertex.pos.y);
			max.z = std::max(max.z, vertex.pos.z);
		}
	}

	if (_meshes.size() == 0) {
		min = _position;
		max = _position;
	}

	// Create or update AABB
	if (_bounds == nullptr) {
		_bounds = new AABB(min, max);
		_worldBounds = new AABB(min, max);
	}
	else {
		_bounds->Min = min;
		_bounds->Max = max;
	}
}

std::vector<iSceneObject*> SceneObject::GetAllObjects() {

	std::vector<iSceneObject*> result;
	for (auto it = _children.begin(); it != _children.end(); it++) {

		std::vector<iSceneObject*> children = it->second->GetAllObjects();
		result.insert(result.end(), children.begin(), children.end());
	}
	result.push_back(this);
	return result;
}

AABB* SceneObject::GetAABB() {

	if (!_bounds)
		return nullptr;

	rml::Matrix model = GetModelMatrix();

	// All corners of the AABB
	std::vector<rml::Vector4> corners = {
	   {_bounds->Min.x, _bounds->Min.y, _bounds->Min.z, 1.0},
	   {_bounds->Min.x, _bounds->Min.y, _bounds->Max.z, 1.0},
	   {_bounds->Min.x, _bounds->Max.y, _bounds->Min.z, 1.0},
	   {_bounds->Min.x, _bounds->Max.y, _bounds->Max.z, 1.0},
	   {_bounds->Max.x, _bounds->Min.y, _bounds->Min.z, 1.0},
	   {_bounds->Max.x, _bounds->Min.y, _bounds->Max.z, 1.0},
	   {_bounds->Max.x, _bounds->Max.y, _bounds->Min.z, 1.0},
	   {_bounds->Max.x, _bounds->Max.y, _bounds->Max.z, 1.0}
	};

	rml::Vector3 min(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()
	);
	rml::Vector3 max(
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest()
	);

	// Apply transformation to every corner.
	// AABB wil become OBB -> recalculate AABB
	for (rml::Vector4& corner : corners) {
		// apply transformation
		corner = corner * model;

		// convert back to AABB
		min.x = std::min(min.x, corner.x);
		min.y = std::min(min.y, corner.y);
		min.z = std::min(min.z, corner.z);

		max.x = std::max(max.x, corner.x);
		max.y = std::max(max.y, corner.y);
		max.z = std::max(max.z, corner.z);
	}

	_worldBounds->Min = min;
	_worldBounds->Max = max;
	return _worldBounds;
}
