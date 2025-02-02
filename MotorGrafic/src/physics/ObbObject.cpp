#include <limits>
#include <cmath>
#include <glad/glad.h>

#include "ObbObject.h"
#include <core/ResourceManager.h>
#include <core/SceneManager.h>
#include <utils/Utils.h>

#include <core/MemoryDebug.h>

ObbObject::ObbObject(unsigned int id, std::string name)
	: SceneObject(id, name, ObjectMovementType::KINEMATIC) {

	Velocity.x = Utils::RandomFloat(-5, 5);
	Velocity.y = Utils::RandomFloat(-5, 5);
	Velocity.z = Utils::RandomFloat(-5, 5);
}

ObbObject::~ObbObject() {

	if (_obbVolume) {
		delete _obbVolume;
		_obbVolume = nullptr;
	}
}

void ObbObject::GenerateObbVolume() {

	_obbVolume = GenerateOBB(_meshes[0]->GetMeshResource()->Vertices);
	GenerateOBBMesh(*_obbVolume);
}

void ObbObject::ComputeCovarianceMatrix(std::vector<rml::Vector3>& centeredVertices, float covariance[3][3]) {

	// Initialize to zero
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			covariance[i][j] = 0.0f;
		}
	}

	// Compute covariance matrix components
	for (const auto& vertex : centeredVertices) {
		covariance[0][0] += vertex.x * vertex.x;
		covariance[0][1] += vertex.x * vertex.y;
		covariance[0][2] += vertex.x * vertex.z;

		covariance[1][0] += vertex.y * vertex.x;
		covariance[1][1] += vertex.y * vertex.y;
		covariance[1][2] += vertex.y * vertex.z;

		covariance[2][0] += vertex.z * vertex.x;
		covariance[2][1] += vertex.z * vertex.y;
		covariance[2][2] += vertex.z * vertex.z;
	}

	// Normalize by the number of vertices
	float invN = 1.0f / static_cast<float>(centeredVertices.size());
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			covariance[i][j] *= invN;
		}
	}
}

rml::Vector3 ObbObject::PowerIteration(float matrix[3][3], int iterations) {
	rml::Vector3 vec(1.0f, 1.0f, 1.0f); // Start with a random vector
	for (int i = 0; i < iterations; ++i) {
		// Multiply the vector by the matrix
		vec = rml::Vector3(
			matrix[0][0] * vec.x + matrix[0][1] * vec.y + matrix[0][2] * vec.z,
			matrix[1][0] * vec.x + matrix[1][1] * vec.y + matrix[1][2] * vec.z,
			matrix[2][0] * vec.x + matrix[2][1] * vec.y + matrix[2][2] * vec.z
		);
		// Normalize the vector
		vec = vec / vec.Length();
	}
	return vec;
}

OBB* ObbObject::GenerateOBB(std::vector<Vertex>& vertices) {
	// Step 1: Calculate the centroid (mean position)
	rml::Vector3 centroid(0, 0, 0);
	for (const auto& vertex : vertices) {
		centroid += vertex.pos;
	}
	centroid /= static_cast<float>(vertices.size());

	// Step 2: Center the vertices
	std::vector<rml::Vector3> centeredVertices;
	for (auto& vertex : vertices) {
		centeredVertices.push_back(vertex.pos - centroid);
	}

	// Step 3: Compute the covariance matrix
	float covariance[3][3];
	ComputeCovarianceMatrix(centeredVertices, covariance);

	// Step 4: Find the principal axes using power iteration
	rml::Vector3 axis1 = PowerIteration(covariance);
	rml::Vector3 axis2, axis3;

	// Use Gram-Schmidt orthogonalization to find other two orthogonal axes
	axis2 = (rml::Vector3(-axis1.y, axis1.x, 0)).Normalize();
	axis3 = axis1.Cross(axis2).Normalize();

	// Step 5: Calculate projections to find extents along each axis
	rml::Vector3 minExtents(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	rml::Vector3 maxExtents(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

	for (auto& vertex : vertices) {
		rml::Vector3 relativePos = vertex.pos - centroid;

		// Project onto each axis
		float projection1 = relativePos.Dot(axis1);
		float projection2 = relativePos.Dot(axis2);
		float projection3 = relativePos.Dot(axis3);

		minExtents.x = std::min(minExtents.x, projection1);
		maxExtents.x = std::max(maxExtents.x, projection1);

		minExtents.y = std::min(minExtents.y, projection2);
		maxExtents.y = std::max(maxExtents.y, projection2);

		minExtents.z = std::min(minExtents.z, projection3);
		maxExtents.z = std::max(maxExtents.z, projection3);
	}

	// Step 6: Set up the OBB structure
	OBB* obb = new OBB();
	obb->center = centroid;
	obb->axes[0] = axis1;
	obb->axes[1] = axis2;
	obb->axes[2] = axis3;
	obb->halfExtents = (maxExtents - minExtents) / 2.0f;

	// Adjust center to be in world coordinates (shift by half-extents in local OBB space)
	obb->center = centroid +
		axis1 * ((maxExtents.x + minExtents.x) / 2.0f) +
		axis2 * ((maxExtents.y + minExtents.y) / 2.0f) +
		axis3 * ((maxExtents.z + minExtents.z) / 2.0f);

	return obb;
}

void ObbObject::GenerateOBBMesh(OBB& obb) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Step 1: Calculate the 8 corner points of the OBB
	rml::Vector3 corners[8];

	// Calculate each corner by combining the center with half-extent displacements along each axis
	corners[0] = obb.center + obb.axes[0] * obb.halfExtents.x + obb.axes[1] * obb.halfExtents.y + obb.axes[2] * obb.halfExtents.z;
	corners[1] = obb.center - obb.axes[0] * obb.halfExtents.x + obb.axes[1] * obb.halfExtents.y + obb.axes[2] * obb.halfExtents.z;
	corners[2] = obb.center + obb.axes[0] * obb.halfExtents.x - obb.axes[1] * obb.halfExtents.y + obb.axes[2] * obb.halfExtents.z;
	corners[3] = obb.center - obb.axes[0] * obb.halfExtents.x - obb.axes[1] * obb.halfExtents.y + obb.axes[2] * obb.halfExtents.z;
	corners[4] = obb.center + obb.axes[0] * obb.halfExtents.x + obb.axes[1] * obb.halfExtents.y - obb.axes[2] * obb.halfExtents.z;
	corners[5] = obb.center - obb.axes[0] * obb.halfExtents.x + obb.axes[1] * obb.halfExtents.y - obb.axes[2] * obb.halfExtents.z;
	corners[6] = obb.center + obb.axes[0] * obb.halfExtents.x - obb.axes[1] * obb.halfExtents.y - obb.axes[2] * obb.halfExtents.z;
	corners[7] = obb.center - obb.axes[0] * obb.halfExtents.x - obb.axes[1] * obb.halfExtents.y - obb.axes[2] * obb.halfExtents.z;

	// Step 2: Define vertices
	vertices.clear();
	for (int i = 0; i < 8; ++i) {
		Vertex vertex;
		vertex.pos = corners[i];
		vertices.push_back(vertex);
	}

	// Step 3: Define indices for the 12 triangles that make up the 6 faces of the box
	// Each face is composed of two triangles (6 faces x 2 triangles = 12 triangles)
	indices = {
		0, 1, 2, 2, 1, 3,  // Front face
		4, 6, 5, 5, 6, 7,  // Back face
		0, 2, 4, 4, 2, 6,  // Top face
		1, 5, 3, 3, 5, 7,  // Bottom face
		0, 4, 1, 1, 4, 5,  // Left face
		2, 3, 6, 6, 3, 7   // Right face
	};

	std::string meshId = "sceneObject_" + std::to_string(_id) + "_obb";
	Mesh* mesh = ResourceManager::GetInstance().CreateAndAddMesh(meshId, vertices, indices);
	/*_obbObj = SceneManager::GetInstance().CreateUsingMesh(
		"box",
		meshId,
		"color",
		rml::Vector3(0, 0, 0),
		rml::Vector3(1, 1, 1),
		rml::Vector3(1, 1, 1),
		ObjectMovementType::SEMI_STATIC,
		nullptr
	);
	AddChild(_obbObj);
	_obbObj->AddMaterial(ResourceManager::GetInstance().GetMaterial("default"));
	_obbObj->SetDrawMode(GL_LINES);*/
}
