#include <iostream>

#include "SceneManager.h"
#include "ResourceManager.h"
#include "SceneXmlParser.h"
#include "Renderer.h"

#include "MemoryDebug.h"

SceneManager* SceneManager::_spInstance = nullptr;
unsigned int SceneManager::_camerasIdCounter = 0;

SceneManager& SceneManager::GetInstance() {
	if (_spInstance == nullptr)
		_spInstance = new SceneManager();
	return *_spInstance;
}

void SceneManager::DestroyInstance() {

	if (_implementation)
		delete _implementation;

	for (auto it = _cameras.begin(); it != _cameras.end(); it++) {
		if (it->second)
			delete it->second;
	}

	DestroyOctree();

	if (_spInstance)
		delete _spInstance;
}

void SceneManager::ParseFile(std::string filepath) {

	// Call parser
	SceneXmlParser::ParseFile(filepath);
}

iSceneObject* SceneManager::CreateAndAddSceneObject(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) {

	return _implementation->CreateAndAddSceneObject(name, modelId, shaderId, position, rotation, scale, movementType, parent);
}
iSceneObject* SceneManager::CreateUsingModel(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) {
	return _implementation->CreateUsingModel(name, modelId, shaderId, position, rotation, scale, movementType, parent);
}
iSceneObject* SceneManager::CreateUsingMesh(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) {
	return _implementation->CreateUsingMesh(name, meshId, shaderId, position, rotation, scale, movementType, parent);
}
LightObject* SceneManager::CreateAndAddLightObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, rml::Vector3 color, ObjectMovementType movementType, iSceneObject* parent) {
	return _implementation->CreateAndAddLightObject(name, meshId, shaderId, position, rotation, scale, color, movementType, parent);
}

void SceneManager::CreateAndAddCameraObject(std::string name, rml::Vector3 pos, rml::Vector3 target, rml::Vector3 up, float translSpeed, float rotationSpeed, float fov, float zNear, float zFar, bool setActive) {
	unsigned int id = _camerasIdCounter++;
	Camera* camera = new Camera(id, name);
	camera->Init(pos, target, up, fov, zNear, zFar);

	_cameras[id] = camera;
	if (setActive) {
		_activeCamera = camera;
	}
}

//TODO: Physics world should not use rendering using octree (addign and removing by moving is expensive)
// So go to Worlds3 and override draw method to draw everything

// Make Octree to be created at start of program. Everytiem an Obejct is added, should be added to octree
// Make as if an object is changing position, it is replaced isnide the octree

void SceneManager::RemoveObject(unsigned int id) {

	auto itKinematic = _kinematicObjects.find(id);
	if (itKinematic != _kinematicObjects.end()) {
		//already deleted by above loop
		_kinematicObjects.erase(itKinematic);
	}

	_implementation->RemoveObject(id);
}

void SceneManager::AddSceneObject(iSceneObject* obj) {
	_implementation->AddSceneObject(obj);
	// We don t use octree to render kinemati cobjects. They are movign a lot, we don t want to update the cotree so much
	if (obj->GetMovementType() == ObjectMovementType::KINEMATIC) {
		_kinematicObjects[obj->GetID()] = obj;
	}
	else {
		InsertObjectToOctree(obj);
	}
}

void SceneManager::AddLightObject(LightObject* obj) {
	assert(_lightObjects.find(obj->GetID()) == _lightObjects.end());
	AddSceneObject(obj);

	_lightObjects[obj->GetID()] = obj;
	// We don t use octree to render kinemati cobjects. They are movign a lot, we don t want to update the cotree so much
	if (obj->GetMovementType() == ObjectMovementType::KINEMATIC) {
		_kinematicObjects[obj->GetID()] = obj;
	}
	else {
		InsertObjectToOctree(obj);
	}
}

Camera* SceneManager::GetCamera(std::string name) {
	for (auto it = _cameras.begin(); it != _cameras.end(); it++) {
		if (it->second->GetName() == name)
			return it->second;
	}
	return nullptr;
}

SceneManager::SceneManager() {

	_implementation = new SceneManagerImplementationBase();
}

void SceneManager::InsertObjectToOctree(iSceneObject* object) {
	if (_rootOctreeNode)
		InsertObjectToOctree(*_rootOctreeNode, object, _octreeMaxDepth);
}

void SceneManager::InsertObjectToOctree(OctreeNode& node, iSceneObject* object, int maxDepth) {

	if (!node.Bounds.Intersects(*object->GetAABB())) {
		return; // Object is outside this node's bounds
	}

	if (node.IsLeaf) {
		if (node.Depth >= maxDepth) {
			// Insert directly if at max depth
			node.Objects.push_back(object);
			return;
		}

		// If object limit exceeded, subdivide and redistribute
		node.Subdivide();
		for (auto* obj : node.Objects) {
			InsertObjectToOctree(*node.Children[OctantFor(node, obj)], obj, maxDepth);
		}
		node.Objects.clear(); // Clear objects from this node after redistributing
	}

	// Insert into relevant child node(s)
	for (auto& child : node.Children) {
		if (child->Bounds.Intersects(*object->GetAABB())) {
			InsertObjectToOctree(*child, object, maxDepth);
			//Without this break here, same object will be stored in more octreeNodes (all of which it intersects)
			break;
		}
	}
}

std::vector<iSceneObject*> SceneManager::GetKinematicObjects() {
	std::vector<iSceneObject*> result;
	for (auto it = _kinematicObjects.begin(); it != _kinematicObjects.end(); it++) {

		std::vector<iSceneObject*> objects = it->second->GetAllObjects();
		result.insert(result.end(), objects.begin(), objects.end());
	}

	return result;
}

std::vector<iSceneObject*> SceneManager::GetAllObjectsInsideFrustrum(OctreeNode& node, Frustrum* frustrum) {

	if (frustrum->IsInFrustrum(node.Bounds)) {
		if (node.IsLeaf) {
			//return objects
			return node.Objects;
		}
		else {
			std::vector<iSceneObject*> objects;
			for (auto& child : node.Children) {
				std::vector<iSceneObject*> temp = GetAllObjectsInsideFrustrum(*child, frustrum);
				objects.insert(objects.end(), temp.begin(), temp.end());
			}
			return objects;
		}
	}
}

int SceneManager::OctantFor(OctreeNode& node, iSceneObject* object) {
	rml::Vector3 center = node.Bounds.Center();
	rml::Vector3 objCenter = object->GetAABB()->Center();

	int octant = 0;
	if (objCenter.x >= center.x) octant |= 1; // Right half
	if (objCenter.y >= center.y) octant |= 2; // Top half
	if (objCenter.z >= center.z) octant |= 4; // Front half

	return octant;
}

void SceneManager::CreateOctree(rml::Vector3 min, rml::Vector3 max, int maxDepth) {

	DestroyOctree();

	AABB aabb(min, max);
	_rootOctreeNode = new OctreeNode(aabb);
	_octreeMaxDepth = maxDepth;

	std::vector<iSceneObject*> allObjects = GetAllObjects();
	for (auto it = allObjects.begin(); it != allObjects.end(); it++) {
		InsertObjectToOctree(*it);
	}
}

void SceneManager::DestroyOctree() {

	if (_rootOctreeNode) {
		DestroyOctree(*_rootOctreeNode);
		delete _rootOctreeNode;
	}
}

void SceneManager::OctreeDebugVisual() {
	if (!_rootOctreeNode)
		return;

	ShowOctree(*_rootOctreeNode);
}

std::vector<iSceneObject*> SceneManager::GetSceneObejctsInsideFrustrum(Camera* camera) {

	std::vector<iSceneObject*> result;

	Frustrum* frustrum = camera->GetFrustrum();
	if (!frustrum)
		return result;

	std::vector<iSceneObject*> test = GetAllObjects();
	for (auto& obj : test) {
		if (frustrum->IsInFrustrum(*obj->GetAABB())) {
			result.push_back(obj);
		}
	}
	return result;

	if (frustrum->IsInFrustrum(_rootOctreeNode->Bounds)) {
		return GetAllObjectsInsideFrustrum(*_rootOctreeNode, frustrum);
	}

	return result;
}

std::vector<iSceneObject*> SceneManager::GetSceneObejctsInsideFrustrumOctree(Camera* camera) {
	std::vector<iSceneObject*> result;

	Frustrum* frustrum = camera->GetFrustrum();
	if (!frustrum || !_rootOctreeNode)
		return result;

	if (frustrum->IsInFrustrum(_rootOctreeNode->Bounds)) {
		return GetAllObjectsInsideFrustrum(*_rootOctreeNode, frustrum);
	}

	return result;
}

std::vector<iSceneObject*> SceneManager::GetAllObjects() {
	return _implementation->GetAllObjects();
}

void SceneManager::ShowOctree(OctreeNode& node) {

	// Step 3: Recursively visit child nodes
	if (!node.IsLeaf) {
		for (const auto& child : node.Children) {
			if (child) { // Ensure the child exists
				ShowOctree(*child);
			}
		}
	}
	else {
		node.Bounds.DrawDebugVisual();
	}
}

void SceneManager::DestroyOctree(OctreeNode& node) {

	if (!node.IsLeaf) {
		for (int i = 0; i < 8; i++) {
			DestroyOctree(*node.Children[i]);
			delete node.Children[i];
		}
	}

	if (node.IsLeaf) {
		// Don't free the memory. SceneManager handles the lifetime of sceneobjects
		node.Objects.clear();
	}
}
