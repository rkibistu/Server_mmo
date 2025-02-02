#include "LightObject.h"
#include <core/SceneManager.h>

LightObject::LightObject(unsigned int id, std::string name, rml::Vector3 color, ObjectMovementType movementType)
	: SceneObject(id, name, movementType), _color(color) {
}

void LightObject::SetCustomUniforms() {

	_shader->SetUniform3f("u_color", _color);
}

