#pragma once

#include <RapidXML/rapidxml.hpp>
#include <string>

#include <components/SceneObjects/iSceneObject.h>

class SceneXmlParser {
public:
	/**
	 * Reads the xml file and populate SceneManager members.
	 * 
	 * 
	 *
	 * \param filename resource xml that will pe parsed
	 * \return succes/failure code
	 */
	static int ParseFile(std::string filepath);

private:
	/**
	 * Read objects node from XML file and call
	 * sceneoBject creation emthod from SceneManager class for every boject.
	 *
	 * \return MY_SUCCES_CODE/MY_ERROR_CODE
	 */
	static int ReadObjects();
	static int ReadLights();

	static int ReadObject(rapidxml::xml_node<>* objectNode, iSceneObject* parent = nullptr);
	static int ReadLightObject(rapidxml::xml_node<>* lightNode, iSceneObject* parent = nullptr);
	/**
	 * Read cameras node from XML file and add them to the SceneManager.
	 * Set the active camera. Default: first one in XML file.
	 * 
	 * \return MY_SUCCES_CODE/MY_ERROR_CODE
	 */
	static int ReadCameras();

	static int ReadDirectionalLight();

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
	static int ReadUnsignedInt(rapidxml::xml_node<>* node, std::string nodeName, unsigned int& result);
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
	static int ReadBoolAttribute(rapidxml::xml_node<>* node, std::string attrName, bool& result);
private:

	//library expect file content to be load until finished with document parsing
	static rapidxml::xml_document<>* _document;
	static rapidxml::xml_node<>* _pRoot;
};
