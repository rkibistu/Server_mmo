#include <iostream>
#include <cassert>
#include <filesystem>

#include "Model.h"
#include <core/Defines.h>
#include <core/ResourceManager.h>

#include <core/MemoryDebug.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define INVALID_MATERIAL 0xFFFFFFFF

Model::Model(ModelResource* modelResource)
	: _modelResource(modelResource) {
}

Model::~Model() {
	if (_modelResource)
		delete _modelResource;

	for (auto it = _meshesMaterials.begin(); it != _meshesMaterials.end(); it++) {
		if (*it)
			delete* it;
	}

	//if (_rootNode) {
	//	for (auto it = _rootNode->Children.begin(); it != _rootNode->Children.end(); it++) {
	//		DestroyChildrernNodes(*it);
	//	}
	//	delete _rootNode;
	//	test--;
	//	std::cout << "Test: " << test << "\n";
	//}
	DestroyChildrernNodes(_rootNode);
}

int Model::Load() {
	// Read file using assimp
	Assimp::Importer Importer;
	std::string filename = _modelResource->FilePath().c_str();
	const aiScene* pScene = Importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);
	if (!pScene) {
		std::cout << "[ASSIMP][ERROR] Fail to read file\n";
		return MY_ERROR_CODE;
	}

	// Loop through all materials and create the textures vector
	// (all textures that exist on this model)
	InitMaterials(pScene);

	// Loop through all meshes and create the _meshesTexture vector
	// (all meshes that exist on the model and their assosiated texture
	// index in the texture vector)
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		Mesh* mesh = InitSingleMesh(paiMesh, _modelResource->ID + "_mesh_" + std::to_string(i));
		mesh->Load();
		//add to internal state
		MeshMaterial* meshMaterial = new MeshMaterial(mesh, paiMesh->mMaterialIndex);
		_meshesMaterials.push_back(meshMaterial);
	}

	// Loop through all nodes and create the ModelNode tree.
	aiNode* paiRootNode = pScene->mRootNode;
	_rootNode = new ModelNode();
	test++;
	for (unsigned int i = 0; i < paiRootNode->mNumMeshes; i++) {
		_rootNode->MeshMaterialIndices.push_back(paiRootNode->mMeshes[i]);
	}
	for (unsigned int i = 0; i < paiRootNode->mNumChildren; i++) {
		InitChildNode(paiRootNode->mChildren[i], _rootNode);
	}

	_loaded = true;
	return MY_SUCCES_CODE;
}

Mesh* Model::GetFirstMesh(std::string meshId) {
	// Read file using assimp
	Assimp::Importer Importer;
	std::string filename = _modelResource->FilePath().c_str();
	const aiScene* pScene = Importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);
	if (!pScene) {
		std::cout << "[ASSIMP][ERROR] Fail to read file\n";
		return MY_ERROR_CODE;
	}


	Mesh* mesh = InitSingleMesh(pScene->mMeshes[0], meshId);

	return mesh;
}

Mesh* Model::InitSingleMesh(const aiMesh* paiMesh, std::string meshId) {
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Vertexes
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D pPos = paiMesh->HasPositions() ? paiMesh->mVertices[i] : Zero3D;
		const aiVector3D pNormal = paiMesh->HasNormals() ? paiMesh->mNormals[i] : Zero3D;
		const aiVector3D pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;

		Vertex temp;
		temp.pos = rml::Vector3(pPos.x, pPos.y, pPos.z);
		temp.norm = rml::Vector3(pNormal.x, pNormal.y, pNormal.z);
		temp.uv = rml::Vector2(pTexCoord.x, pTexCoord.y);
		vertices.push_back(temp);
	}

	//Indices
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& face = paiMesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++) {

			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* mesh = ResourceManager::GetInstance().CreateAndAddMesh(meshId, vertices, indices);

	return mesh;
}

void Model::InitMaterials(const aiScene* pScene) {

	// Loop though all materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		std::string materialID = _modelResource->ID + "_material_" + std::to_string(i);
		Material* material = ResourceManager::GetInstance().CreateAndAddMaterial(materialID);

		// Load diffuse color
		aiColor3D color;
		if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
			material->DiffuseColor = rml::Vector3(color.r, color.g, color.b);
		}

		// Load ambient color
		if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
			material->AmbientColor = rml::Vector3(color.r, color.g, color.b);
		}

		// Load specular color
		if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
			material->SpecularColor = rml::Vector3(color.r, color.g, color.b);
		}

		// Load emissive color
		if (pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
			material->EmissiveColor = rml::Vector3(color.r, color.g, color.b);
		}

		// Load shininess
		float shininess;
		if (pMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
			material->Shininess = shininess;
		}

		// Load opacity
		float opacity;
		if (pMaterial->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
			material->Opacity = opacity;
		}

		// Get texture details if the diffuse texture exist inside the material.
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			// Get path of the texture
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p(Path.data);
				std::filesystem::path textureFilename(Path.data);
				std::string texturePath = "";

				if (!textureFilename.is_absolute()) {
					// If relative, resolve the path relative to the model directory
					texturePath = _modelResource->Path;
				}

				// Create texture
				std::string textureID = materialID + "_diffusetexture";
				Texture* diffuseTexture = ResourceManager::GetInstance().CreateAndAddTexture(
					texturePath,
					textureFilename.string(),
					TEXTURE_TYPE_2D,
					"LINEAR",
					"LINEAR",
					"GL_REPEAT",
					"GL_REPEAT",
					materialID
				);

				// Load the texture
				if (diffuseTexture->Load() != MY_SUCCES_CODE) {
					std::cout << "[IMPORTER][ERROR] Failed to load texture: " << textureFilename.string() << "\n";
					return;
				}

				material->DiffuseTexture = diffuseTexture;
				std::cout << "[IMPORTER][INFO] loaded texture: " << textureFilename.string() << "\n";
			}
		}
		else {
			// Default texture
			material->DiffuseTexture = ResourceManager::GetInstance().GetTexture("default");
		}
		// Adds the texture the the vector
		// Adds nullptr even if the material doesn't have a diffuse texture
		// because we want to keep the itnernal mesh-texture idnexing
		// (every aiMesh has a mMaterialIndex to assosiace with a material)
		_materials.push_back(material);
	};
}

void Model::InitChildNode(const aiNode* paiNode, ModelNode* parentNode) {

	ModelNode* modelNode = new ModelNode();
	test++;
	for (unsigned int i = 0; i < paiNode->mNumMeshes; i++) {
		modelNode->MeshMaterialIndices.push_back(paiNode->mMeshes[i]);
	}
	for (unsigned int i = 0; i < paiNode->mNumChildren; i++) {
		InitChildNode(paiNode->mChildren[i], modelNode);
	}
	parentNode->Children.push_back(modelNode);
}

void Model::DestroyChildrernNodes(ModelNode* parentNode) {
	if (!parentNode)
		return;

	for (ModelNode* child : parentNode->Children) {
		DestroyChildrernNodes(child);
	}

	delete parentNode;
	test--;
}
