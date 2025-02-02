#pragma once

#include <utils/Math.h>
#include <components/Texture.h>

struct Material{
	friend class ResourceManager;

private:
	// Private constructors so only ResourceManager can create resources 
	Material() { ; }
	Material(const Material&) = default;
	Material& operator=(const Material&) = default;
public:
	std::string ID;

	rml::Vector3 AmbientColor;
	rml::Vector3 DiffuseColor;
	rml::Vector3 SpecularColor;
	rml::Vector3 EmissiveColor;
	float Shininess = 0.0f;
	float Opacity = 1.0f;

	Texture* DiffuseTexture = nullptr;

	// Not used yet
	Texture* SpecularTexture = nullptr;
	// Not used yet
	Texture* NormalMap = nullptr;
};
