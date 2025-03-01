#pragma once

#include <unordered_map>

#include "core/culling/Octree.h"
#include <components/Model.h>
#include <components/SceneObjects/iSceneObject.h>
#include <components/Camera.h>
#include <components/LightObject.h>
#include "core/culling/Frustrum.h"

#include "SceneManagers/SceneManagerImplementationBase.h"

/**
 * Singleton class used to manage all objects that exist in the world.
 */
class SceneManager {

public:
	static SceneManager& GetInstance();
	void DestroyInstance();

	/** Parse .xml file and create all objects */
	void ParseFile(std::string filepath);

	/** Creates and adds to the manager a scene object */
	iSceneObject* CreateAndAddSceneObject(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent = nullptr);
	/** Creates an object with all information from the model, including children hierrachy and materials(textures) */
	iSceneObject* CreateUsingModel(std::string name, std::string modelId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent = nullptr);
	/** Creates an object with only a mesh, no materials/textures */
	iSceneObject* CreateUsingMesh(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, ObjectMovementType movementType, iSceneObject* parent);
	/** Creates and adds to the manager a light object */
	LightObject* CreateAndAddLightObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, rml::Vector3 color, ObjectMovementType movementType, iSceneObject* parent);
	/**
	 * Creates and adds to the manager a new camera.
	 *
	 * \param name the name of the new camera
	 * \param pos the initial position
	 * \param target the target to look at
	 * \param up internal up vector of the camera
	 * \param translSpeed movement speed
	 * \param rotationSpeed rotation speed
	 * \param fov
	 * \param zNear
	 * \param zFar
	 * \param setActive if True -> this camera will be set up as the active camera
	 */
	void CreateAndAddCameraObject(std::string name, rml::Vector3 pos, rml::Vector3 target, rml::Vector3 up, float translSpeed, float rotationSpeed, float fov, float zNear, float zFar, bool setActive = false);

	/**
	 * Remove object from object vectors.
	 * There are multiple vectors, but the id is unique through all of them.
	 * 
	 * \param id Id of the object to remove
	 */
	void RemoveObject(unsigned int id);
	void AddSceneObject(iSceneObject* obj);
	void AddLightObject(LightObject* obj);

	std::unordered_map<unsigned int, iSceneObject*>& GetSceneObjects() { return _implementation->GetSceneObjects(); }
	std::unordered_map<unsigned int, LightObject*>& GetLightObjects() { return _implementation->GetLightObjects(); }
	inline Camera* GetActivecamera() { return _activeCamera; }
	Camera* GetCamera(std::string name);

	/** Returns next id that should be used to create a sceneObject. It auto increments. */
	inline unsigned int GetSceneObjectID() { return _implementation->GetSceneObjectID(); }

	/** Distribute all objects in the octree */
	void CreateOctree(rml::Vector3 min, rml::Vector3 max, int maxDepth);
	void DestroyOctree();
	void OctreeDebugVisual();
	/** Returns all objects that intersects the frustrum of the given camera */
	std::vector<iSceneObject*> GetSceneObejctsInsideFrustrum(Camera* camera);
	/** Returns all objects that are inside an OctreeNode that intersects the frustrum of the given camera */
	std::vector<iSceneObject*> GetSceneObejctsInsideFrustrumOctree(Camera* camera);
	/** Returns all objects, not only root objects */
	std::vector<iSceneObject*> GetAllObjects();
	/** Returns all objects that are kinematic objects (kinemaitc objects + all their chidlren) */
	std::vector<iSceneObject*> GetKinematicObjects();
public:
	rml::Vector3 DirectionalLightDirection = rml::Vector3(1, -1, 1);
	rml::Vector3 DirectionalLightColor = rml::Vector3(1, 1, 1);

private:
	void InsertObjectToOctree(iSceneObject* object);

	void InsertObjectToOctree(OctreeNode& node, iSceneObject* object, int maxDepth);

	std::vector<iSceneObject*> GetAllObjectsInsideFrustrum(OctreeNode& node, Frustrum* frustrum);
	
	/** Decide in which child node of the octree node shpuld this object be placed */
	int OctantFor(OctreeNode& node, iSceneObject* object);

	void ShowOctree(OctreeNode& node);
	void DestroyOctree(OctreeNode& node);


private:
	static SceneManager* _spInstance;
	SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator= (const SceneManager&) = delete;

	/**
	 * Just a reference to the kinematic objects. Memory her eshould never be allcoate/deallocate. Just point to existing ones..
	 * It is used to render all kinematic objects without using the octree.
	 */
	std::unordered_map<unsigned int, iSceneObject*> _kinematicObjects;
	OctreeNode* _rootOctreeNode = nullptr;
	int _octreeMaxDepth = 3;

	// This is just for structure. You should not have objects here that are not in _sceneObjects
	std::unordered_map<unsigned int, LightObject*> _lightObjects;
	iSceneManagerImplementation* _implementation = nullptr;

	Camera* _activeCamera = nullptr;
	std::unordered_map<unsigned int, Camera*> _cameras;
	static unsigned int _camerasIdCounter;
};