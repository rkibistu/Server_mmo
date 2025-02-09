#include "Player.h"

#include "core/SceneManager.h"
#include "core/ResourceManager.h"

Player::Player(int id, std::string username, rml::Vector3 position)
	: _id(id), _username(username), _position(position) {

	Spawn();
}

void Player::Spawn() {

	Mesh* mesh = ResourceManager::GetInstance().GetMesh("1");
	GenerateBoundingVolume(mesh);
}

void Player::GenerateBoundingVolume(Mesh* mesh) {
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

	// Iterate over all vertices of the mesh
	for (const Vertex& vertex : mesh->GetMeshResource()->Vertices) {
		// Update min and max bounds based on vertex positions
		min.x = std::min(min.x, vertex.pos.x);
		min.y = std::min(min.y, vertex.pos.y);
		min.z = std::min(min.z, vertex.pos.z);

		max.x = std::max(max.x, vertex.pos.x);
		max.y = std::max(max.y, vertex.pos.y);
		max.z = std::max(max.z, vertex.pos.z);
	}

	_boundingVolume.Min = min;
	_boundingVolume.Max = max;
}
