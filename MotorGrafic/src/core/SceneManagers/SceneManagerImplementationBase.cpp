#include "SceneManagerImplementationBase.h"

#include <components/SceneObject.h>

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
