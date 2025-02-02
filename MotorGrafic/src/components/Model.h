#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

#include "Mesh.h"
#include "Material.h"

/**
 * Structure that represents the data found in resources.xml. This data is used to find
 * the model filepath.
 */
struct ModelResource {

	std::string ID;
	std::string Path;
	std::string Filename;

	std::string FilePath() { return Path + Filename; }
};

/**
 * A model can have multiple children nodes, like a hierarchy.
 * Every node contains a list of indices used to identify all of the meshes
 * stored in _meshesMaterials vector from model class.
 * 
 * Not every node must have a visual representation (mesh), but
 * every node will be used to create a scene object because a node 
 * can be a joint, a point of rotation, etc. So MeshMaterialIndices can be 
 * empty.
 */
struct ModelNode {

	std::vector<unsigned int> MeshMaterialIndices;
	std::vector<ModelNode*> Children;
};

/**
 * A structure to link a Mesh to his specific Material.
 * 
 * Every mesh has associatted an index. This is the index of the associatted material
 * found in _materials vector of Model class. That vector contains all materials used
 * by this model.
 * 
 * Not every mesh has a material and a material can be used by multiple meshes.
 */
struct MeshMaterial {
	MeshMaterial(Mesh* mesh, unsigned int materialIndex) {
		Mesh = mesh;
		MaterialIndex = materialIndex;
	}

	Mesh* Mesh;
	unsigned int MaterialIndex;
};

/**
 * It is a RESOURCE that represents a collection of Meshes and Textures that assemble
 * a 3D complex object loaded from a file (like .obj).
 * 
 * It is a resource that is used to create sceneobjects, not an actual 
 * sceneobject. It can be used by multiple sceneobjects.
 */
class Model {
	friend class ResourceManager;

private:
	// Private constructors so only ResourceManager can create resources 
	Model() = delete;
	Model(ModelResource* modelResource);
	Model(const Model&) = default;
	Model& operator=(const Model&) = default;
public:
	~Model();

	int Load();
	/**
	 * Returns only the first mesh of the model and don't add any information to model internal state.
	 * The mesh is not loaded. 
	 * 
	 * \param meshId desired meshId
	 * \return 
	 */
	Mesh* GetFirstMesh(std::string meshId);

	bool IsLoaded() { return _loaded; }
	
	ModelNode* GetRootNode() { return _rootNode; }
	Mesh* GetMesh(unsigned int index) { return _meshesMaterials[index]->Mesh; }
	Material* GetMaterial(unsigned int index) { return _meshesMaterials[index]->MaterialIndex < _materials.size() ? _materials[_meshesMaterials[index]->MaterialIndex] : nullptr; }
	inline std::string GetID() { return _modelResource->ID; }
private:
	Mesh* InitSingleMesh(const aiMesh* paiMesh, std::string meshId);
	void InitMaterials(const aiScene* pScene);
	void InitChildNode(const aiNode* paiNode, ModelNode* parentNode);
	
	/** Destroy all ModelNode pointers that are children to this parentNode */
	void DestroyChildrernNodes(ModelNode* parentNode);

private:
	ModelResource* _modelResource = nullptr;

	// If True -> it means all ModelData children are loaded
	bool _loaded = false;

	/**
	 * Vector of Meshes and their associatted index texture.
	 */
	std::vector<MeshMaterial*> _meshesMaterials;
	/**
	 * Vector of all Materials sued on this model.
	 * Link between meshes and amterials is done using _meshesMaterials array
	 */
	std::vector<Material*> _materials;

	ModelNode* _rootNode = nullptr;
	unsigned int test = 0;
};