#include <glad/glad.h>

#include "AABB.h"
#include <core/SceneManager.h>
#include <core/Renderer.h>

#include <core/MemoryDebug.h>

AABB::AABB(rml::Vector3 min, rml::Vector3 max) 
	: Min(min), Max(max) {
}

AABB::~AABB() {

}

bool AABB::Contains(const AABB& other) const {
	return (other.Min.x >= Min.x && other.Max.x <= Max.x &&
		other.Min.y >= Min.y && other.Max.y <= Max.y &&
		other.Min.z >= Min.z && other.Max.z <= Max.z);
}

bool AABB::Intersects(const AABB& other) const {
	return (Min.x <= other.Max.x && Max.x >= other.Min.x &&
		Min.y <= other.Max.y && Max.y >= other.Min.y &&
		Min.z <= other.Max.z && Max.z >= other.Min.z);
}

void AABB::DrawDebugVisual() {

	Renderer::DrawBox(
		Min,
		Max,
		rml::Vector3(1, 0, 1),
		GL_LINES
	);
}

