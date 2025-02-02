#pragma once

#include "iSceneMangerImplementation.h"

class SceneManagerImplementationBase : public iSceneManagerImplementation {

	std::unordered_map<unsigned int, iSceneObject*>& GetSceneObjects() { return _sceneObjects; }
	std::unordered_map<unsigned int, LightObject*>& GetLightObjects() { return _lightObjects; }
	/** Returns all objects, not only root objects */
	std::vector<iSceneObject*> GetAllObjects();

	inline unsigned int GetSceneObjectID() override { return _sceneObjectIdCounter++; }

protected:
	virtual ~SceneManagerImplementationBase();

	// This vector manages the lifetime of the objects
	// All obejcts should be here. Other vectors are just refrences ti this.
	std::unordered_map<unsigned int, iSceneObject*> _sceneObjects;
	static unsigned int _sceneObjectIdCounter;

	// This is just for structure. You should not have objects here that are not in _sceneObjects
	std::unordered_map<unsigned int, LightObject*> _lightObjects;
};
