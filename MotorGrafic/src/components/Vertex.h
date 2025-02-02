#pragma once

#include <utils/Math.h>

/** Structure that represent a vertex of a 3D mesh */
struct Vertex {
	rml::Vector3 pos;
	rml::Vector2 uv;
	rml::Vector3 norm;
	rml::Vector3 color;
	rml::Vector3 binorm;
};

/**
 * This is used when creating vertex array buffer. This order should be kept in shaders too.
 */
enum VertexAttribLocation {
	POS		=	0,
	UV		=	1,
	NORM	=	2,
	COLOR	=	3,
	BINORM	=	4
};
