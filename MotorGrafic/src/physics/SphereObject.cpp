#include <iostream>


#include "SphereObject.h"
#include <components/Mesh.h>
#include <utils/Utils.h>

#include <core/MemoryDebug.h>

SphereObject::SphereObject(unsigned int id, std::string name)
	: SceneObject(id, name, ObjectMovementType::KINEMATIC) {

	Velocity.x = Utils::RandomFloat(-15, 15);
	Velocity.y = Utils::RandomFloat(-2, 25);
	Velocity.z = Utils::RandomFloat(-15, 15);
}

SphereObject::~SphereObject() {
	if (_boundingSphere) {
		delete _boundingSphere;
		_boundingSphere = nullptr;
	}
}

void SphereObject::GenerateBoundingSphere() {

	if (_meshes.size() <= 0) {

		_boundingSphere = new BoundingSphere{ {0, 0, 0}, 0 };  // Empty mesh edge case
		std::cout << "[SPHEREOBJECT][INFO]: No meshes -> create empty bounding sphere.\n";
		return;
	}

	rml::Vector3 center;
	float radius;

	//Calcualte center. We assume that all objects that simulate physics have a single mesh
	for (auto& vertex : _meshes[0]->GetMeshResource()->Vertices) {
		center = center + vertex.pos;
	}
	center = center / static_cast<float>(_meshes[0]->GetMeshResource()->Vertices.size());


	// Calculate the radius
	radius = 0.0f;
	rml::Vector3 diff;
	for (auto& vertex : _meshes[0]->GetMeshResource()->Vertices) {
		diff = center - vertex.pos;
		float distance = diff.Length();
		if (distance > radius) {
			radius = distance;
		}
	}

	_boundingSphere = new BoundingSphere{ center, radius };
}
