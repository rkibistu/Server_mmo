#pragma once

#include <vector>

#include "core/AABB.h"
#include <components/SceneObject.h>

struct OctreeNode {

	OctreeNode(const AABB& bounds, int depth = 0);

	void Subdivide();

	std::vector<iSceneObject*> Objects;
	AABB Bounds;
	std::vector<OctreeNode*> Children;
	// depth level of this node
	int Depth;
	// True if it has no children (not subdivided yet, all children are nullptr)
	bool IsLeaf;
};
