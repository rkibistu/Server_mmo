#pragma once

#include <components/SceneObject.h>

struct OBB {
	rml::Vector3 center;     // Center of the OBB
	rml::Vector3 axes[3];    // Local axes (directions of the OBB)
	rml::Vector3 halfExtents; // Half-width along each axis
};

class ObbObject : public SceneObject {
	friend class PhysicsController;

public:
	ObbObject(unsigned int id, std::string name);
	~ObbObject();

	void GenerateObbVolume();

	rml::Vector3 Velocity = rml::Vector3(0, 0, 0);

private:
	void ComputeCovarianceMatrix(std::vector<rml::Vector3>& centeredVertices, float covariance[3][3]);
	rml::Vector3 PowerIteration(float matrix[3][3], int iterations = 50);
	OBB* GenerateOBB(std::vector<Vertex>& vertices);

	void GenerateOBBMesh(OBB& obb);

private:
	OBB* _obbVolume = nullptr;
	//SceneObject* _obbObj = nullptr;
};
