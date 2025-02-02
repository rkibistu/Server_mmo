#pragma once

#include "utils/Math.h"
#include <unordered_map>
#include <vector>

#include "core/AABB.h"
#include "core/Renderer.h"

class Camera;

struct Plane {
	rml::Vector3 normal; // Plane normal
	rml::Vector3 point;
	float d;     // Distance from origin

	// Plane equation: A * X + B * Y + C * Z + D = 0
	float distanceToPoint( rml::Vector3& p) {
		return normal.Dot(p - point);
	}
};


class Frustrum {

public:
	Frustrum();

	bool IsInFrustrum(AABB& aabb);

	// Computing the signed distance tells us which side of the plane the point is on.
	// Assuming that the planes' normals are pointing inwards,
	// then if the signed distance is negative the point is outside the frustum. 
	// Otherwise the point is on the right side of the plane.
	bool IsInFrustrum(rml::Vector3 point);

	void GenerateCorners(Camera* camera);

	void Draw();

private:
	//std::array<Plane, 6> _planes;
	std::unordered_map<std::string, Plane> _planes;
	// f - far; t - top; b - bottom; l - left; r - right
	rml::Vector3 _ftl, _ftr, _fbl, _fbr, _ntl, _ntr, _nbl, _nbr;
};
