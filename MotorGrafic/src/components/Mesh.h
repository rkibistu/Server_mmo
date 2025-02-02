#pragma once

#include <vector>
#include <string>

#include "Vertex.h"

/**
 * Structure that represents the static data, read from file, 
 * that describes a mesh.
 */
struct MeshResource {

	std::string ID;
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<unsigned int> WiredIndices;
};

/**
 * It is a RESOURCE that represents a colleciton of vertices that defines a 3D object.
 * 
 * A mesh can't be bind (used) before being loaded (by calling
 * Load method)
 * 
 * This is a resource, it can be used by multiple sceneobjects to render 
 * same mesh in diffrent contexts. 
 */
class Mesh {
	friend class ResourceManager;
	friend class World;
	friend class Renderer;

private:
	// Private constructors so only ResourceManager can create resources 
	Mesh() = delete;
	Mesh(MeshResource* meshResource);
	Mesh(const Mesh&) = default;
	Mesh& operator=(const Mesh&) = default;

public:
	~Mesh();

	/**
	 * Load opengl structures using data from MeshResource.
	 * A Mesh can't be bind before being loaded.
	 * 
	 * \return 
	 * - MY_SUCCESS_CODE: Succes - mesh was laoded successfully
	 * - MY_ERROR_CIDE: Error - mesh was not loaded
	 */
	int Load();

	void BindFilled();
	void BindWired();
	void Unbind();

	void Draw(int drawMode);

	inline MeshResource* GetMeshResource() { return _meshResource; }
	inline unsigned int GetIndicesFilledCount() { return (_meshResource != nullptr) ? _meshResource->Indices.size() : 0; }
	inline unsigned int GetIndicesWiredCount() { return (_meshResource != nullptr) ? _meshResource->WiredIndices.size() : 0; }

	inline 	bool IsLoaded() { return _loaded; }

private:
	//get indices for triangle mode and create indices for wired mode
	void CreateWiredindicesBuffer(std::vector<unsigned int>& indices, std::vector<unsigned int>& wiredIndices);

	//Bind and load to opengl the vertices and idncies buffers
	//	get values from _modelResource
	void BindAndLoadVertices();


private:
	MeshResource* _meshResource = nullptr;

	/**
	 * False at creation. True after the Mesh is laoded successfully
	 * after a call of Load method.
	 */
	bool _loaded = false;

	/** Only VAO is used to bind. Other are used to construct VAO */
	unsigned int _VBO, _VAO, _EBO;
};
