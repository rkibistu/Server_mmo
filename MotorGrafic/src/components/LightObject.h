#pragma once

#include "SceneObject.h"

class LightObject : public SceneObject{
public:
	LightObject(unsigned int id, std::string name, rml::Vector3 color, ObjectMovementType movementType = ObjectMovementType::SEMI_STATIC);

	inline rml::Vector3 GetColor() { return _color; }

	void SetCustomUniforms() override;

private:
	rml::Vector3 _color;
};
