#pragma once

#include "iSceneMangerImplementation.h"

class Model;
class ModelNode;

class SceneManagerImplementationBase : public iSceneManagerImplementation {
public:
	virtual ~SceneManagerImplementationBase();

	std::unordered_map<unsigned int, iSceneObject*>& GetSceneObjects() { return _sceneObjects; }
	std::unordered_map<unsigned int, LightObject*>& GetLightObjects() { return _lightObjects; }
	/** Returns all objects, not only root objects */
	std::vector<iSceneObject*> GetAllObjects() override;

	inline unsigned int GetSceneObjectID() override { return _sceneObjectIdCounter++; }

	/** Creates and adds to the manager a scene object */
	iSceneObject* CreateAndAddSceneObject(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent = nullptr) override;
	/** Creates an object with all information from the model, including children hierrachy and materials(textures) */
	iSceneObject* CreateUsingModel(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent = nullptr) override;
	/** Creates an object with only a mesh, no materials/textures */
	iSceneObject* CreateUsingMesh(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) override;
	/** Creates and adds to the manager a light object */
	LightObject* CreateAndAddLightObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, rml::Vector3 color, ObjectMovementType movementType, iSceneObject* parent) override;

	/**
	 * Remove object from object vectors.
	 * There are multiple vectors, but the id is unique through all of them.
	 *
	 * \param id Id of the object to remove
	 */
	void RemoveObject(unsigned int id) override;
	void AddSceneObject(iSceneObject* obj) override;
	void AddLightObject(LightObject* obj) override;

protected:
	/** Used to loop through all children of a model and create children scene objects */
	void CreateSceneObjectChild(Model* model, ModelNode* modelNode, iSceneObject* parent);

protected:

	// This vector manages the lifetime of the objects
	// All obejcts should be here. Other vectors are just refrences ti this.
	std::unordered_map<unsigned int, iSceneObject*> _sceneObjects;
	static unsigned int _sceneObjectIdCounter;

	// This is just for structure. You should not have objects here that are not in _sceneObjects
	std::unordered_map<unsigned int, LightObject*> _lightObjects;
};
