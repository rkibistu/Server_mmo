#pragma once

#include <utils/Math.h>

class SceneObject;

class AABB {

public:
	AABB(rml::Vector3 min, rml::Vector3 max);
	~AABB();

	rml::Vector3 Min; // Minimum (bottom-left-back) point of the box
	rml::Vector3 Max; // Maximum (top-right-front) point of the box

	rml::Vector3 Center() const { return (Min + Max) / 2; }

	// Check if an object with its bounding box is within this AABB
	bool Contains(const AABB& other) const;

	// Check if this AABB intersects another AABB
	bool Intersects(const AABB& other) const;

	void DrawDebugVisual();
};