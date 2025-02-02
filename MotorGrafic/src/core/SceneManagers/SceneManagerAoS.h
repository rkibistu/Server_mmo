#pragma once

#include "SceneManagerImplementationBase.h"

class Model;
class ModelNode;

// Data is stores as an array oj objects
class SceneManagerAoS : public SceneManagerImplementationBase {

public:
	virtual ~SceneManagerAoS() { ; }

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

private:
	/** Used to loop through all children of a model and create children scene objects */
	void CreateSceneObjectChild(Model* model, ModelNode* modelNode, iSceneObject* parent);
};
