#include <glad/glad.h>

#include "BoxObject.h"
#include <utils/Utils.h>
#include <core/SceneManager.h>
#include <core/ResourceManager.h>
#include <core/Renderer.h>

#include <core/MemoryDebug.h>

BoxObject::BoxObject(unsigned int id, std::string name)
	: SceneObject(id, name, ObjectMovementType::KINEMATIC) {

	Velocity.x = Utils::RandomFloat(-15, 15);
	Velocity.y = Utils::RandomFloat(-2, 25);
	Velocity.z = Utils::RandomFloat(-15, 15);
}

BoxObject::~BoxObject() {
	if (_boundingVolume) {
		delete _boundingVolume;
		_boundingVolume = nullptr;
	}
}

void BoxObject::CustomDraw() {
}

void BoxObject::GenerateBoundingVolume() {

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
	_boundingVolume = new BoundingVolume{ min,max };
}
