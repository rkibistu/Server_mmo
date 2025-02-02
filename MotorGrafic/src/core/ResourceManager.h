#pragma once

#include <unordered_map>
#include <string>

#include <components/Model.h>
#include <components/Texture.h>
#include <components/Shader.h>

class ResourceXmlParser;

/**
 * Singleton class used to manage the resources.
 * 
 * Resources are reusable data and opengl structures that are used in creation
 * of scene objects (models, textures, shaders, etc.) .
 * It intends to load data only on demand. 
 * 
 * ATTENTION: Only this class should allocate and deallocate resources.
 * Other classes should use the data, but no other class should allocate
 * or deallocate a resource.
 * 
 * Later, this will parse an .xml to get all the ifnromation needed to load
 * data when necessary.
 * Later, we could unload data if they are not used.
 */
class ResourceManager {
	friend class ResourceXmlParser;

public:
	static ResourceManager& GetInstance();
	void DestroyInstance();

	/**
	 * Parse the .xml file and store all data needed so
	 * the resources can be laoded when they are accessed.
	 */
	void ParseFile(std::string filepath);

	/**
	 * Initiate the loading process.
	 * 
	 * \param id the ID of the resource
	 */
	void LoadTexture(std::string id);
	void LoadModel(std::string id);
	void LoadShader(std::string id);

	/**
	 * Gets the specific resource.
	 * If it is not already loaded, it will load the resource
	 * before returning it.
	 * 
	 * \param id the ID of the resource
	 * \return a pointer to the resource
	 */
	Texture* GetTexture(std::string id);
	Model* GetModel(std::string id);
	Shader* GetShader(std::string id);
	Mesh* GetMesh(std::string id);
	Material* GetMaterial(std::string id);

	Material* CreateAndAddMaterial(std::string id);
	Texture* CreateAndAddTexture(std::string path, std::string filename, std::string type, std::string minFilter, std::string magFilter, std::string wrapS, std::string wrapT, std::string id);
	Mesh* CreateAndAddMesh(std::string id, std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	/**
	 * Read a model file (like .obj) and create only one mesh (first one enctountered).
	 * This should be used with modle files that have only one mesh for best results.
	 * 
	 * \param id desired id of the resource
	 * \param filename
	 * \param filepath
	 * \return a pointer to the created resource
	 */
	Mesh* CreateAndAddMesh(std::string id, std::string filename, std::string filepath);
private:
	void CreateAndAddModel(std::string filename, std::string filepath, std::string id);
	void CreateAndAddShader(std::string path, std::string filenameVS, std::string filenameFS, std::string id);
	template <typename T>
	void Clear(std::unordered_map<std::string, T>& unorderedMap) {

		for (auto it = unorderedMap.begin(); it != unorderedMap.end(); ++it) {
			delete it->second;
		}
		unorderedMap.clear();
	}
private:
	static ResourceManager* _spInstance;
	ResourceManager() { ; }
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator= (const ResourceManager&) = delete;

	/** Resources */
	std::unordered_map<std::string, Material*> _materials;
	std::unordered_map<std::string, Texture*> _textures;
	std::unordered_map<std::string, Model*> _models;
	std::unordered_map<std::string, Shader*> _shaders;
	std::unordered_map<std::string, Mesh*> _meshes;
};