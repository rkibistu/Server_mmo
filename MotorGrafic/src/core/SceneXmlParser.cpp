#include<sstream>
#include <fstream>
#include <iostream>

#include "SceneXmlParser.h"
#include "SceneManager.h"
#include <core/Defines.h>

#include "MemoryDebug.h"

#define XML_GAME_NAME_NODE				"gameName"
#define XML_SCREEN_SIZE_NODE			"defaultScreenSize"
#define XML_BACKGROUND_COLOR_NODE		"backgroundColor"
#define XML_CAMERAS_ROOT				"cameras"
#define XML_CAMERA_NODE					"camera"
#define XML_POSITION_NODE				"position"
#define XML_ROTATION_NODE				"rotation"
#define XML_SCALE_NODE					"scale"
#define XML_TARGET_NODE					"target"
#define XML_UP_NODE						"up"
#define XML_TRANSLATION_SPEED_NODE		"translationSpeed"
#define XML_ROTATION_SPEED_NODE			"rotationSpeed"
#define XML_FOV_NODE					"fov"
#define XML_ZNEAR_NODE					"near"
#define XML_ZFAR_NODE					"far"
#define XML_OBJECTS_ROOT				"objects"
#define XML_OBJECT_NODE					"object"
#define XML_MODEL_ID_NODE				"model"
#define XML_SHADER_ID_NODE				"shader"
#define XML_NAME_NODE					"name"
#define XML_CHILDREN_NODE				"children"
#define XML_LIGHTS_ROOT					"lights"
#define XML_LIGHT_NODE					"light"
#define XML_MESH_ID_NODE				"mesh"
#define XML_COLOR_NODE					"color"
#define XML_DIRECTIONAL_LIGHT_NODE		"directionalLight"
#define XML_DIRECTION_NODE				"direction"
#define XML_ID_ATTRIBUTE				"id"
#define XML_ACTIVE_ATTRIUBTE			"active"

#define XML_X_NODE						"x"
#define XML_Y_NODE						"y"
#define XML_Z_NODE						"z"
#define XML_R_NODE						"r"
#define XML_G_NODE						"g"
#define XML_B_NODE						"b"

rapidxml::xml_document<>* SceneXmlParser::_document = nullptr;
rapidxml::xml_node<>* SceneXmlParser::_pRoot = nullptr;

int SceneXmlParser::ParseFile(std::string filepath) {
	// Open xml file
	std::fstream file(filepath);
	if (!file.is_open()) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Configure file '" << filepath << "' doesn't exist!" << std::endl;
		return MY_ERROR_CODE;
	}

	// Read file
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	file.close();

	// Init structures
	_document = new rapidxml::xml_document<>();
	_document->parse<0>(&content[0]);
	_pRoot = _document->first_node();

	int ret;
	ret = ReadObjects();
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadCameras();
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadLights();
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadDirectionalLight();
	if (ret != MY_SUCCES_CODE) return ret;

	delete _document;

	return MY_SUCCES_CODE;
}

// PRIVATE

int SceneXmlParser::ReadObjects() {

	if (_pRoot == nullptr) {
		std::cout << "[SCENEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	int ret = MY_SUCCES_CODE;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_OBJECTS_ROOT);
	if (!pNode) {
		std::cout << "[SCENEXMLPARSER][INFO]: No scene objects in xml file.";
		return MY_ERROR_CODE;
	}

	//for  every <XML_OBJECT_NODE>
	for (rapidxml::xml_node<>* objectNode = pNode->first_node(XML_OBJECT_NODE); objectNode; objectNode = objectNode->next_sibling(XML_OBJECT_NODE)) {
		//read the object
		ReadObject(objectNode, nullptr);
	}

	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadLights() {
	if (_pRoot == nullptr) {
		std::cout << "[SCENEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	int ret = MY_SUCCES_CODE;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_LIGHTS_ROOT);
	if (!pNode) {
		std::cout << "[SCENEXMLPARSER][INFO]: No light objects in xml file.";
		return MY_ERROR_CODE;
	}

	//for  every <XML_OBJECT_NODE>
	for (rapidxml::xml_node<>* lightNode = pNode->first_node(XML_LIGHT_NODE); lightNode; lightNode = lightNode->next_sibling(XML_LIGHT_NODE)) {
		//read the object
		ReadLightObject(lightNode, nullptr);
	}

	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadObject(rapidxml::xml_node<>* objectNode, iSceneObject* parent) {

	int ret = MY_SUCCES_CODE;
	std::string modelId;
	std::string shaderId;
	std::string objectName;
	rml::Vector3 position;
	rml::Vector3 rotation;
	rml::Vector3 scale;

	// Read every data about the boject
	ret = ReadString(objectNode, XML_MODEL_ID_NODE, modelId);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadString(objectNode, XML_SHADER_ID_NODE, shaderId);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadString(objectNode, XML_NAME_NODE, objectName);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadVector3_xyz(objectNode, XML_POSITION_NODE, position);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadVector3_xyz(objectNode, XML_ROTATION_NODE, rotation);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadVector3_xyz(objectNode, XML_SCALE_NODE, scale);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;

	// Create the object and add it to the SceneManager
	iSceneObject* sceneObject = SceneManager::GetInstance().CreateAndAddSceneObject(
		objectName,
		modelId,
		shaderId,
		position,
		rotation,
		scale,
		ObjectMovementType::SEMI_STATIC,
		parent
	);

	// Parse and create all children objects (if exist)
	rapidxml::xml_node<>* childrenNode = objectNode->first_node(XML_CHILDREN_NODE);
	if (childrenNode) {
		for (rapidxml::xml_node<>* childObjectNode = childrenNode->first_node(XML_OBJECT_NODE); childObjectNode; childObjectNode = childObjectNode->next_sibling(XML_OBJECT_NODE)) {
			ReadObject(childObjectNode, sceneObject);
		}
	}

	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadLightObject(rapidxml::xml_node<>* lightNode, iSceneObject* parent) {
	int ret = MY_SUCCES_CODE;
	std::string meshId;
	std::string shaderId;
	std::string name;
	rml::Vector3 position;
	rml::Vector3 rotation;
	rml::Vector3 scale;
	rml::Vector3 color;

	// Read every data about the boject
	ret = ReadString(lightNode, XML_MESH_ID_NODE, meshId);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadString(lightNode, XML_SHADER_ID_NODE, shaderId);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadString(lightNode, XML_NAME_NODE, name);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadVector3_xyz(lightNode, XML_POSITION_NODE, position);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadVector3_xyz(lightNode, XML_ROTATION_NODE, rotation);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadVector3_xyz(lightNode, XML_SCALE_NODE, scale);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;
	ret = ReadVector3_rgb(lightNode, XML_COLOR_NODE, color);
	if (ret != MY_SUCCES_CODE) return MY_ERROR_CODE;

	// Create the object and add it to the SceneManager
	SceneObject* sceneObject = SceneManager::GetInstance().CreateAndAddLightObject(
		name,
		meshId,
		shaderId,
		position,
		rotation,
		scale,
		color,
		ObjectMovementType::SEMI_STATIC,
		parent
	);

	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadCameras() {
	if (_pRoot == nullptr) {
		std::cout << "[SCENEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	int ret = MY_SUCCES_CODE;
	std::string cameraName;
	rml::Vector3 cameraPos;
	rml::Vector3 cameraTerget;
	rml::Vector3 cameraUp;
	float cameraTranslationSpeed;
	float cameraRotationSpeed;
	float cameraFov;
	float cameraZnear;
	float cameraZfar;
	bool activeCamera;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_CAMERAS_ROOT);
	if (!pNode) {
		std::cout << "[SCENEXMLPARSER][INFO]: No scene objects in xml file.";
		return MY_ERROR_CODE;
	}

	//for  every <XML_CAMERA_NODE>
	for (rapidxml::xml_node<>* cameraNode = pNode->first_node(XML_CAMERA_NODE); cameraNode; cameraNode = cameraNode->next_sibling(XML_CAMERA_NODE)) {

		activeCamera = false;
		ret = ReadBoolAttribute(cameraNode, XML_ACTIVE_ATTRIUBTE, activeCamera);

		ret = ReadString(cameraNode, XML_NAME_NODE, cameraName);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadVector3_xyz(cameraNode, XML_POSITION_NODE, cameraPos);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadVector3_xyz(cameraNode, XML_TARGET_NODE, cameraTerget);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadVector3_xyz(cameraNode, XML_UP_NODE, cameraUp);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadFloat(cameraNode, XML_TRANSLATION_SPEED_NODE, cameraTranslationSpeed);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadFloat(cameraNode, XML_ROTATION_SPEED_NODE, cameraRotationSpeed);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadFloat(cameraNode, XML_FOV_NODE, cameraFov);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadFloat(cameraNode, XML_ZNEAR_NODE, cameraZnear);
		if (ret != MY_SUCCES_CODE) continue;
		ret = ReadFloat(cameraNode, XML_ZFAR_NODE, cameraZfar);
		if (ret != MY_SUCCES_CODE) continue;

		SceneManager::GetInstance().CreateAndAddCameraObject(
			cameraName,
			cameraPos,
			cameraTerget,
			cameraUp,
			cameraTranslationSpeed,
			cameraRotationSpeed,
			cameraFov,
			cameraZnear,
			cameraZfar,
			activeCamera
		);
	}

	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadDirectionalLight() {
	if (_pRoot == nullptr) {
		std::cout << "[SCENEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_DIRECTIONAL_LIGHT_NODE);
	if (!pNode) {
		std::cout << "[SCENEXMLPARSER][INFO]: No DirectionalLight in xml file.";
		return MY_ERROR_CODE;
	}

	int ret;
	rml::Vector3 direction;
	rml::Vector3 color;

	ret = ReadVector3_xyz(pNode, XML_DIRECTION_NODE, direction);
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadVector3_rgb(pNode, XML_COLOR_NODE, color);
	if (ret != MY_SUCCES_CODE) return ret;

	SceneManager::GetInstance().DirectionalLightDirection = direction;
	SceneManager::GetInstance().DirectionalLightColor = color;

	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadString(rapidxml::xml_node<>* node, std::string nodeName, std::string& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing child node from node <" << node->name() << ">. Missing child : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	result = searchedNode->value();
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadInt(rapidxml::xml_node<>* node, std::string nodeName, int& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing child node from node <" << node->name() << ">. Missing child : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	result = atoi(searchedNode->value());
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadUnsignedInt(rapidxml::xml_node<>* node, std::string nodeName, unsigned int& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing child node from node <" << node->name() << ">. Missing child : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	result = atoi(searchedNode->value());
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadFloat(rapidxml::xml_node<>* node, std::string nodeName, float& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing child node from node <" << node->name() << ">. Missing child : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	result = atof(searchedNode->value());
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadVector3_xyz(rapidxml::xml_node<>* node, std::string nodeName, rml::Vector3& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing child node from node <" << node->name() << ">. Missing child : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	int ret;
	float x = 0, y = 0, z = 0;

	ret = ReadFloat(searchedNode, XML_X_NODE, x);
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadFloat(searchedNode, XML_Y_NODE, y);
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadFloat(searchedNode, XML_Z_NODE, z);
	if (ret != MY_SUCCES_CODE) return ret;

	result.x = x;
	result.y = y;
	result.z = z;
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadVector3_rgb(rapidxml::xml_node<>* node, std::string nodeName, rml::Vector3& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing child node from node <" << node->name() << ">. Missing child : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	int ret;
	float r = 0, g = 0, b = 0;

	ret = ReadFloat(searchedNode, XML_R_NODE, r);
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadFloat(searchedNode, XML_G_NODE, g);
	if (ret != MY_SUCCES_CODE) return ret;
	ret = ReadFloat(searchedNode, XML_B_NODE, b);
	if (ret != MY_SUCCES_CODE) return ret;

	result.x = r;
	result.y = g;
	result.z = b;
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadStringAttribute(rapidxml::xml_node<>* node, std::string attrName, std::string& result) {
	rapidxml::xml_attribute<>* searchedAttr = node->first_attribute(attrName.c_str());
	if (!searchedAttr) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing attribute from node <" << node->name() << ">. Missing attribute : <" << attrName << "> \n";
		return MY_ERROR_CODE;
	}

	result = searchedAttr->value();
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadUnsignedIntAttribute(rapidxml::xml_node<>* node, std::string attrName, unsigned int& result) {
	rapidxml::xml_attribute<>* searchedAttr = node->first_attribute(attrName.c_str());
	if (!searchedAttr) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing attribute from node <" << node->name() << ">. Missing attribute : <" << attrName << "> \n";
		return MY_ERROR_CODE;
	}

	result = atoi(searchedAttr->value());
	return MY_SUCCES_CODE;
}

int SceneXmlParser::ReadBoolAttribute(rapidxml::xml_node<>* node, std::string attrName, bool& result) {
	rapidxml::xml_attribute<>* searchedAttr = node->first_attribute(attrName.c_str());
	if (!searchedAttr) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing attribute from node <" << node->name() << ">. Missing attribute : <" << attrName << "> \n";
		return MY_ERROR_CODE;
	}

	result = false;
	if (strcmp(searchedAttr->value(), "true") == 0 || strcmp(searchedAttr->value(), "True") == 0) {
		result = true;
	}

	return MY_SUCCES_CODE;
}
