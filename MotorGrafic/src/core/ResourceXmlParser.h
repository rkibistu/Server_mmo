#pragma once

#include <RapidXML/rapidxml.hpp>
#include <string>

#include <components/Model.h>
#include <components/Shader.h>
#include <components/Texture.h>


class ResourceXmlParser{
public:
	/**
	 * Reads the file and prepare internal structure for parsing.
	 * 
	 * \param filename resource xml that will pe parsed
	 * \return succes/failure code
	 */
	static int ParseFile(std::string filepath);


private:
	/** Read specific entities nodes and add them to resource manager */
	static int ReadModels();
	static int ReadShaders();
	static int ReadTextures();
	static int ReadMeshes();
	static int ReadMaterials();

	/**
	 * Get a child node value.
	 * 
	 * \param node parent node inside the search is made
	 * \param nodeName name of the node we search for
	 * \param result out param, the value of found node
	 * \return MY_SUCCES_CODE/MY_ERROR_CODE
	 */
	static int ReadString(rapidxml::xml_node<>* node, std::string nodeName, std::string& result);
	static int ReadInt(rapidxml::xml_node<>* node, std::string nodeName, int& result);
	static int ReadFloat(rapidxml::xml_node<>* node, std::string nodeName, float& result);
	static int ReadVector3_xyz(rapidxml::xml_node<>* node, std::string nodeName, rml::Vector3& result);
	static int ReadVector3_rgb(rapidxml::xml_node<>* node, std::string nodeName, rml::Vector3& result);

	/**
	 * Get attribute value from node.
	 *
	 * \param node parent node inside the search is made
	 * \param attrName name of the attribute we search for
	 * \param result out param, the value of found attribute
	 * \return MY_SUCCES_CODE/MY_ERROR_CODE
	 */
	static int ReadStringAttribute(rapidxml::xml_node<>* node, std::string attrName, std::string& result);
	static int ReadUnsignedIntAttribute(rapidxml::xml_node<>* node, std::string attrName, unsigned int& result);
private:

	//library expect file content to be load until finished with document parsing
	static rapidxml::xml_document<>* _document;
	static rapidxml::xml_node<>* _pRoot;
};
