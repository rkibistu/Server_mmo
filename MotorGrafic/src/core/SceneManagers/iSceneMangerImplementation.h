#pragma once

#include <utils/Math.h>

#include <components/SceneObjects/iSceneObject.h>
#include <unordered_map>
#include <string>

class iSceneObject;
class LightObject;
class Camera;

class iSceneManagerImplementation {

public:
	virtual ~iSceneManagerImplementation() { ; }

	/** Creates and adds to the manager a scene object */
	virtual iSceneObject* CreateAndAddSceneObject(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent = nullptr) = 0;
	/** Creates an object with all information from the model, including children hierrachy and materials(textures) */
	virtual iSceneObject* CreateUsingModel(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent = nullptr) = 0;
	/** Creates an object with only a mesh, no materials/textures */
	virtual iSceneObject* CreateUsingMesh(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent) = 0;
	/** Creates and adds to the manager a light object */
	virtual LightObject* CreateAndAddLightObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, rml::Vector3 color, ObjectMovementType movementType, iSceneObject* parent) = 0;

	/**
	 * Remove object from object vectors.
	 * There are multiple vectors, but the id is unique through all of them.
	 *
	 * \param id Id of the object to remove
	 */
	virtual void RemoveObject(unsigned int id) = 0;
	virtual void AddSceneObject(iSceneObject* obj) = 0;
	virtual void AddLightObject(LightObject* obj) = 0;

	virtual std::unordered_map<unsigned int, iSceneObject*>& GetSceneObjects() = 0;
	virtual std::unordered_map<unsigned int, LightObject*>& GetLightObjects() = 0;
	/** Returns all objects, not only root objects */
	virtual std::vector<iSceneObject*> GetAllObjects() = 0;

	/** Returns next id that should be used to create a sceneObject. It auto increments. */
	virtual inline unsigned int GetSceneObjectID() = 0;
};
