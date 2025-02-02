#include<sstream>
#include <fstream>
#include <iostream>

#include "ResourceXmlParser.h"
#include <core/Defines.h>
#include "ResourceManager.h"

#include "MemoryDebug.h"

#define XML_MODELS_ROOT					"models"
#define XML_SHADERS_ROOT				"shaders"
#define XML_TEXTURES_ROOT				"textures"
#define XML_MESHES_ROOT					"meshes"
#define XML_MATERIALS_ROOT				"materials"
#define XML_FOLDER_ROOT					"folder"
#define XML_PATH_ATTRIBUTE				"path"
#define XML_FILE_NODE					"file"
#define XML_ID_ATTRIBUTE				"id"
#define XML_VS_FILE_NODE				"vs" //vertexshader
#define XML_FS_FILE_NODE				"fs" //frag shader
#define XML_TYPE_ATTRIBUTE				"type"
#define XML_TEXTURE_MIN_FILTER			"min_filter"
#define XML_TEXTURE_MAG_FILTER			"mag_filter"
#define XML_TEXTURE_WRAP_S				"wrap_s"
#define XML_TEXTURE_WRAP_T				"wrap_t"
#define XML_MODEL_NODE					"model"
#define XML_SHADER_NODE					"shader"
#define XML_TEXTURE_NODE				"texture"
#define XML_MESH_NODE					"mesh"
#define XML_MATERIAL_NODE				"material"
#define XML_AMBIENT_COLOR_NODE			"ambientColor"
#define XML_DIFFUSE_COLOR_NODE			"diffuseColor"
#define XML_SPECULAR_COLOR_NODE			"specularColor"
#define XML_EMISSIVE_COLOR_NODE			"emissiveColor"
#define XML_SHININESS_NODE				"shininess"
#define XML_OPACITY_NODE				"opacity"
#define XML_DIFFUSE_TEXTURE_ID_NODE		"diffuseTextureId"
#define XML_SPECULAR_TEXTURE_ID_NODE	"specularTextureId"
#define XML_NORMALMAP_TEXTURE_ID_NODE	"normalmapTextureId"

#define XML_X_NODE						"x"
#define XML_Y_NODE						"y"
#define XML_Z_NODE						"z"
#define XML_R_NODE						"r"
#define XML_G_NODE						"g"
#define XML_B_NODE						"b"

rapidxml::xml_document<>* ResourceXmlParser::_document = nullptr;
rapidxml::xml_node<>* ResourceXmlParser::_pRoot = nullptr;

int ResourceXmlParser::ParseFile(std::string filepath) {
	// Open xml file
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cout << "Configure file '" << filepath << "' doesn't exist!" << std::endl;
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

	ResourceXmlParser::ReadTextures();
	ResourceXmlParser::ReadMeshes();
	ResourceXmlParser::ReadShaders();
	ResourceXmlParser::ReadModels();
	ResourceXmlParser::ReadMaterials();

	delete _document;

	return MY_SUCCES_CODE;
}

// PRIVATE

int ResourceXmlParser::ReadModels() {

	if (_pRoot == nullptr) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	int ret;
	std::string modelPath;
	std::string modelFilename;
	std::string modelID;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_MODELS_ROOT);
	if (!pNode) {
		std::cout << "[RESOURCEXMLPARSER][INFO]: No models in xml file.";
		return MY_ERROR_CODE;
	}
	//for  every <folder>
	for (rapidxml::xml_node<>* folerNode = pNode->first_node(); folerNode; folerNode = folerNode->next_sibling()) {
		//check to be correct node <XML_FOLDER_ROOT>
		if (std::strcmp(folerNode->name(), XML_FOLDER_ROOT) != 0) {
			std::cout << "[RESOURCEXMLPARSER][ERROR]: Wrong node under models node. Found: <" << folerNode->name() << ">. It should be only: <" << XML_FOLDER_ROOT << "> \n";
			continue;
		}

		//check for path attribute
		ret = ReadStringAttribute(folerNode, XML_PATH_ATTRIBUTE, modelPath);
		if (ret != MY_SUCCES_CODE) continue;

		//loop every model node
		for (rapidxml::xml_node<>* modelNode = folerNode->first_node(); modelNode; modelNode = modelNode->next_sibling()) {
			//check to be correct node <XML_MODEL_NODE>
			if (std::strcmp(modelNode->name(), XML_MODEL_NODE) != 0) {
				std::cout << "[RESOURCEXMLPARSER][ERROR]: Wrong node under folder node. Found: <" << modelNode->name() << ">. It should be only: <" << XML_MODEL_NODE << "> \n";
				continue;
			}

			//check for if attribute
			ret = ReadStringAttribute(modelNode, XML_ID_ATTRIBUTE, modelID);
			if (ret != MY_SUCCES_CODE) continue;

			//get filename node
			ret = ReadString(modelNode, XML_FILE_NODE, modelFilename);
			if (ret != MY_SUCCES_CODE) continue;

			ResourceManager::GetInstance().CreateAndAddModel(modelFilename, modelPath, modelID);
		}
	}

	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadShaders() {
	if (_pRoot == nullptr) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	int ret;
	std::string shaderPath;
	std::string vertexShaderFilename;;
	std::string fragmentShaderFilename;;
	std::string shaderID;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_SHADERS_ROOT);
	if (!pNode) {
		std::cout << "[RESOURCEXMLPARSER][INFO]: No shaders in xml file.";
		return MY_ERROR_CODE;
	}
	//for  every <folder>
	for (rapidxml::xml_node<>* folerNode = pNode->first_node(); folerNode; folerNode = folerNode->next_sibling()) {
		//check to be correct node <XML_FOLDER_ROOT>
		if (std::strcmp(folerNode->name(), XML_FOLDER_ROOT) != 0) {
			std::cout << "[RESOURCEXMLPARSER][ERROR]: Wrong node under shaders node. Found: <" << folerNode->name() << ">. It should be only: <" << XML_FOLDER_ROOT << "> \n";
			continue;
		}

		//check for path attribute
		ret = ReadStringAttribute(folerNode, XML_PATH_ATTRIBUTE, shaderPath);
		if (ret != MY_SUCCES_CODE) continue;

		//loop every shader
		for (rapidxml::xml_node<>* shaderNode = folerNode->first_node(); shaderNode; shaderNode = shaderNode->next_sibling()) {
			//check to be correct node <XML_SHADER_NODE>
			if (std::strcmp(shaderNode->name(), XML_SHADER_NODE) != 0) {
				std::cout << "[RESOURCEXMLPARSER][ERROR]: Wrong node under folder node. Found: <" << shaderNode->name() << ">. It should be only: <" << XML_SHADER_NODE << "> \n";
				continue;
			}

			//check for if attribute
			ret = ReadStringAttribute(shaderNode, XML_ID_ATTRIBUTE, shaderID);
			if (ret != MY_SUCCES_CODE) continue;

			ret = ReadString(shaderNode, XML_VS_FILE_NODE, vertexShaderFilename);
			if (ret != MY_SUCCES_CODE) continue;

			ReadString(shaderNode, XML_FS_FILE_NODE, fragmentShaderFilename);
			if (ret != MY_SUCCES_CODE) continue;

			ResourceManager::GetInstance().CreateAndAddShader(shaderPath, vertexShaderFilename, fragmentShaderFilename, shaderID);
		}
	}

	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadTextures() {
	if (_pRoot == nullptr) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	bool ret;
	std::string texturePath;
	std::string textureFilename;;
	std::string textureType;
	std::string minFilter;
	std::string magFilter;
	std::string wrapS;
	std::string wrapT;
	std::string textureID;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_TEXTURES_ROOT);
	if (!pNode) {
		std::cout << "[RESOURCEXMLPARSER][INFO]: No textures in xml file.";
		return MY_ERROR_CODE;
	}
	//for  every <folder>
	for (rapidxml::xml_node<>* folerNode = pNode->first_node(); folerNode; folerNode = folerNode->next_sibling()) {
		//check to be correct node <XML_FOLDER_ROOT>
		if (std::strcmp(folerNode->name(), XML_FOLDER_ROOT) != 0) {
			std::cout << "[RESOURCEXMLPARSER][ERROR]: Wrong node under shaders node. Found: <" << folerNode->name() << ">. It should be only: <" << XML_FOLDER_ROOT << "> \n";
			continue;
		}

		//check for path attribute
		ret = ReadStringAttribute(folerNode, XML_PATH_ATTRIBUTE, texturePath);
		if (ret != MY_SUCCES_CODE) continue;

		//loop every shader
		for (rapidxml::xml_node<>* textureNode = folerNode->first_node(); textureNode; textureNode = textureNode->next_sibling()) {
			//check to be correct node <XML_TEXTURE_NODE>
			if (std::strcmp(textureNode->name(), XML_TEXTURE_NODE) != 0) {
				std::cout << "[RESOURCEXMLPARSER][ERROR]: Wrong node under folder node. Found: <" << textureNode->name() << ">. It should be only: <" << XML_TEXTURE_NODE << "> \n";
				continue;
			}

			//check for attributes
			ret = ReadStringAttribute(textureNode, XML_ID_ATTRIBUTE, textureID);
			if (ret != MY_SUCCES_CODE) continue;
			ret = ReadStringAttribute(textureNode, XML_TYPE_ATTRIBUTE, textureType);
			if (ret != MY_SUCCES_CODE) continue;

			//get texture filename
			ret = ReadString(textureNode, XML_FILE_NODE, textureFilename);
			if (ret != MY_SUCCES_CODE) continue;
			ret = ReadString(textureNode, XML_TEXTURE_MIN_FILTER, minFilter);
			if (ret != MY_SUCCES_CODE) continue;
			ret = ReadString(textureNode, XML_TEXTURE_MAG_FILTER, magFilter);
			if (ret != MY_SUCCES_CODE) continue;
			ret = ReadString(textureNode, XML_TEXTURE_WRAP_S, wrapS);
			if (ret != MY_SUCCES_CODE) continue;
			ret = ReadString(textureNode, XML_TEXTURE_WRAP_T, wrapT);
			if (ret != MY_SUCCES_CODE) continue;

			ResourceManager::GetInstance().CreateAndAddTexture(texturePath, textureFilename, textureType, minFilter, magFilter, wrapS, wrapT, textureID);
		}
	}

	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadMeshes() {
	if (_pRoot == nullptr) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	bool ret;
	std::string id;
	std::string path;
	std::string filename;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_MESHES_ROOT);
	if (!pNode) {
		std::cout << "[RESOURCEXMLPARSER][INFO]: No meshses in xml file.";
		return MY_ERROR_CODE;
	}

	//for  every <folder>
	for (rapidxml::xml_node<>* folerNode = pNode->first_node(XML_FOLDER_ROOT); folerNode; folerNode = folerNode->next_sibling(XML_FOLDER_ROOT)) {

		//check for path attribute
		ret = ReadStringAttribute(folerNode, XML_PATH_ATTRIBUTE, path);
		if (ret != MY_SUCCES_CODE) continue;

		//loop every mesh
		for (rapidxml::xml_node<>* meshNode = folerNode->first_node(XML_MESH_NODE); meshNode; meshNode = meshNode->next_sibling(XML_MESH_NODE)) {

			//check for id attribute
			ret = ReadStringAttribute(meshNode, XML_ID_ATTRIBUTE, id);
			if (ret != MY_SUCCES_CODE) continue;

			ret = ReadString(meshNode, XML_FILE_NODE, filename);
			if (ret != MY_SUCCES_CODE) continue;

			//Readfile -> generate vertices
			ResourceManager::GetInstance().CreateAndAddMesh(id, filename, path);
		}
	}

	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadMaterials() {
	if (_pRoot == nullptr) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: pRoot for xml parser is null! Make sure Init method ran successfully." << std::endl;
		return MY_ERROR_CODE;
	}

	int ret;
	std::string id;
	rml::Vector3 ambientColor = rml::Vector3(1, 1, 1);
	rml::Vector3 diffuseColor = rml::Vector3(1, 1, 1);;
	rml::Vector3 specularColor = rml::Vector3(0, 0, 0);;
	rml::Vector3 emissiveColor = rml::Vector3(0, 0, 0);;
	float shininess = 0.0;
	float opacity = 1.0;
	std::string diffuseTextureId;
	std::string specularTextureId;
	std::string normalMapTextureId;

	rapidxml::xml_node<>* pNode = _pRoot->first_node(XML_MATERIALS_ROOT);
	if (!pNode) {
		std::cout << "[RESOURCEXMLPARSER][INFO]: No materials in xml file.";
		return MY_ERROR_CODE;
	}

	//loop every shader
	for (rapidxml::xml_node<>* materialNode = pNode->first_node(XML_MATERIAL_NODE); materialNode; materialNode = materialNode->next_sibling(XML_MATERIAL_NODE)) {

		//reset values for next material
		ambientColor = rml::Vector3(1, 1, 1);
		diffuseColor = rml::Vector3(1, 1, 1);;
		specularColor = rml::Vector3(0, 0, 0);
		emissiveColor = rml::Vector3(0, 0, 0);
		shininess = 0.0;
		opacity = 1.0;
		diffuseTextureId = "";
		specularTextureId = "";
		normalMapTextureId = "";

		ret = ReadStringAttribute(materialNode, XML_ID_ATTRIBUTE, id);
		if (ret != MY_SUCCES_CODE) continue;

		//Read values. None of them are mandatory
		ret = ReadVector3_rgb(materialNode, XML_AMBIENT_COLOR_NODE, ambientColor);
		ret = ReadVector3_rgb(materialNode, XML_DIFFUSE_COLOR_NODE, diffuseColor);
		ret = ReadVector3_rgb(materialNode, XML_SPECULAR_COLOR_NODE, specularColor);
		ret = ReadVector3_rgb(materialNode, XML_EMISSIVE_COLOR_NODE, emissiveColor);
		ret = ReadFloat(materialNode, XML_SHININESS_NODE, shininess);
		ret = ReadFloat(materialNode, XML_OPACITY_NODE, opacity);
		ret = ReadString(materialNode, XML_DIFFUSE_TEXTURE_ID_NODE, diffuseTextureId);
		ret = ReadString(materialNode, XML_SPECULAR_TEXTURE_ID_NODE, specularTextureId);
		ret = ReadString(materialNode, XML_NORMALMAP_TEXTURE_ID_NODE, normalMapTextureId);

		//Create and add material
		Material* material = ResourceManager::GetInstance().CreateAndAddMaterial(id);
		material->ID = id;
		material->AmbientColor = ambientColor;
		material->DiffuseColor = diffuseColor;
		material->SpecularColor = specularColor;
		material->EmissiveColor = emissiveColor;
		material->Shininess = shininess;
		material->Opacity = opacity;
		Texture* diffuseTexture = ResourceManager::GetInstance().GetTexture(diffuseTextureId);
		material->DiffuseTexture = diffuseTexture != NULL ? diffuseTexture : ResourceManager::GetInstance().GetTexture("default");
		material->SpecularTexture = ResourceManager::GetInstance().GetTexture(specularTextureId);
		material->NormalMap = ResourceManager::GetInstance().GetTexture(normalMapTextureId);
	}

	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadString(rapidxml::xml_node<>* node, std::string nodeName, std::string& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: Missing attribute from node <" << node->name() << ">. Missing attribute : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	result = searchedNode->value();
	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadInt(rapidxml::xml_node<>* node, std::string nodeName, int& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: Missing attribute from node <" << node->name() << ">. Missing attribute : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	result = atoi(searchedNode->value());
	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadFloat(rapidxml::xml_node<>* node, std::string nodeName, float& result) {
	rapidxml::xml_node<>* searchedNode = node->first_node(nodeName.c_str());
	if (!searchedNode) {
		std::cout << "[SCENEXMLPARSER][ERROR]: Missing child node from node <" << node->name() << ">. Missing child : <" << nodeName << "> \n";
		return MY_ERROR_CODE;
	}

	result = atof(searchedNode->value());
	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadVector3_xyz(rapidxml::xml_node<>* node, std::string nodeName, rml::Vector3& result) {
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

int ResourceXmlParser::ReadVector3_rgb(rapidxml::xml_node<>* node, std::string nodeName, rml::Vector3& result) {
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


int ResourceXmlParser::ReadStringAttribute(rapidxml::xml_node<>* node, std::string attrName, std::string& result) {
	rapidxml::xml_attribute<>* searchedAttr = node->first_attribute(attrName.c_str());
	if (!searchedAttr) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: Missing attribute from node <" << node->name() << ">. Missing attribute : <" << attrName << "> \n";
		return MY_ERROR_CODE;
	}

	result = searchedAttr->value();
	return MY_SUCCES_CODE;
}

int ResourceXmlParser::ReadUnsignedIntAttribute(rapidxml::xml_node<>* node, std::string attrName, unsigned int& result) {
	rapidxml::xml_attribute<>* searchedAttr = node->first_attribute(attrName.c_str());
	if (!searchedAttr) {
		std::cout << "[RESOURCEXMLPARSER][ERROR]: Missing attribute from node <" << node->name() << ">. Missing attribute : <" << attrName << "> \n";
		return MY_ERROR_CODE;
	}

	result = atoi(searchedAttr->value());
	return MY_SUCCES_CODE;
}