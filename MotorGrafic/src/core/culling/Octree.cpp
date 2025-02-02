#include <glad/glad.h>

#include "Octree.h"
#include <core/SceneManager.h>

#include <core/MemoryDebug.h>

OctreeNode::OctreeNode(const AABB& bounds, int depth)
	: Bounds(bounds), Depth(depth), IsLeaf(true) {
	Children.reserve(8);
	for (int i = 0; i < 8; ++i) {
        Children.push_back(nullptr);
	}
}

void OctreeNode::Subdivide() {
    if (!IsLeaf) return; // Already subdivided

    rml::Vector3 center = {
        (Bounds.Min.x + Bounds.Max.x) / 2,
        (Bounds.Min.y + Bounds.Max.y) / 2,
        (Bounds.Min.z + Bounds.Max.z) / 2
    };

    // Create 8 child nodes

    Children[0] = new OctreeNode(AABB{ Bounds.Min, center }, Depth + 1); // Bottom-left-back
    Children[1] = new OctreeNode(AABB{ { center.x, Bounds.Min.y, Bounds.Min.z }, { Bounds.Max.x, center.y, center.z } }, Depth + 1); // Bottom-right-back
    Children[2] = new OctreeNode(AABB{ { Bounds.Min.x, center.y, Bounds.Min.z }, { center.x, Bounds.Max.y, center.z } }, Depth + 1); // Top-left-back
    Children[3] = new OctreeNode(AABB{ { center.x, center.y, Bounds.Min.z }, { Bounds.Max.x, Bounds.Max.y, center.z } }, Depth + 1); // Top-right-back
    Children[4] = new OctreeNode(AABB{ { Bounds.Min.x, Bounds.Min.y, center.z }, { center.x, center.y, Bounds.Max.z } }, Depth + 1); // Bottom-left-front
    Children[5] = new OctreeNode(AABB{ { center.x, Bounds.Min.y, center.z }, { Bounds.Max.x, center.y, Bounds.Max.z } }, Depth + 1); // Bottom-right-front
    Children[6] = new OctreeNode(AABB{ { Bounds.Min.x, center.y, center.z }, { center.x, Bounds.Max.y, Bounds.Max.z } }, Depth + 1); // Top-left-front
    Children[7] = new OctreeNode(AABB{ center, Bounds.Max }, Depth + 1); // Top-right-front

    IsLeaf = false;
}

