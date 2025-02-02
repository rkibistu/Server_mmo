#include <iostream>

#include "ResourceManager.h"
#include "ResourceXmlParser.h"
#include <core/Defines.h>

#include "MemoryDebug.h"

ResourceManager* ResourceManager::_spInstance = nullptr;

ResourceManager& ResourceManager::GetInstance() {

	if (_spInstance == nullptr)
		_spInstance = new ResourceManager();
	return *_spInstance;
}

void ResourceManager::DestroyInstance() {

	Clear(_materials);
	Clear(_textures);
	Clear(_models);
	Clear(_shaders);
	Clear(_meshes);

	if (_spInstance)
		delete _spInstance;
}

void ResourceManager::ParseFile(std::string filepath) {

	ResourceXmlParser::ParseFile(filepath);
}

void ResourceManager::LoadTexture(std::string id) {

	_textures[id]->Load();
}

void ResourceManager::LoadModel(std::string id) {
	_models[id]->Load();
}

void ResourceManager::LoadShader(std::string id) {
	_shaders[id]->Load();
}

Texture* ResourceManager::GetTexture(std::string id) {
	if (_textures.find(id) == _textures.end())
		return nullptr;

	int ret = MY_SUCCES_CODE;
	if (!_textures[id]->IsLoaded())
		ret = _textures[id]->Load();

	if (ret != MY_SUCCES_CODE)
		return nullptr;

	return _textures[id];
}

Model* ResourceManager::GetModel(std::string id) {
	if (_models.find(id) == _models.end())
		return nullptr;

	int ret = MY_SUCCES_CODE;
	if (!_models[id]->IsLoaded())
		ret = _models[id]->Load();

	if (ret != MY_SUCCES_CODE)
		return nullptr;

	return _models[id];
}

Shader* ResourceManager::GetShader(std::string id) {
	if (_shaders.find(id) == _shaders.end())
		return nullptr;

	int ret = MY_SUCCES_CODE;
	if (!_shaders[id]->IsLoaded())
		ret = _shaders[id]->Load();

	if (ret != MY_SUCCES_CODE)
		return nullptr;

	return _shaders[id];
}

Mesh* ResourceManager::GetMesh(std::string id) {
	if (_meshes.find(id) == _meshes.end())
		return nullptr;

	int ret = MY_SUCCES_CODE;
	if (!_meshes[id]->IsLoaded())
		ret = _meshes[id]->Load();

	if (ret != MY_SUCCES_CODE)
		return nullptr;

	return _meshes[id];
}

Material* ResourceManager::GetMaterial(std::string id) {
	if (_materials.find(id) == _materials.end())
		return nullptr;

	return _materials[id];
}

Material* ResourceManager::CreateAndAddMaterial(std::string id) {

	if (_materials.find(id) != _materials.end()) {
		std::cout << "[RESOURCE][WARNING]: A material with the same id already existed so it was deleted! Id: " << id << "\n";
		delete _materials[id];
	}

	Material* material = new Material();
	_materials[id] = material;

	return material;
}

Texture* ResourceManager::CreateAndAddTexture(std::string path, std::string filename, std::string type, std::string minFilter, std::string magFilter, std::string wrapS, std::string wrapT, std::string id) {
	if (_textures.find(id) != _textures.end()) {
		std::cout << "[RESOURCE][WARNING]: A texture with the same id already existed so it was deleted! Id: " << id << "\n";
		delete _textures[id];
	}

	TextureResource* textureResource = new TextureResource();
	textureResource->Path = path;
	textureResource->Filename = filename;
	textureResource->Type = type;
	textureResource->MinFilter = minFilter;
	textureResource->MagFilter = magFilter;
	textureResource->WrapS = wrapS;
	textureResource->WrapT = wrapT;
	textureResource->ID = id;
	Texture* texture = new Texture(textureResource);

	_textures[id] = texture;
	return texture;
}

Mesh* ResourceManager::CreateAndAddMesh(std::string id, std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
	if (_meshes.find(id) != _meshes.end()) {
		std::cout << "[RESOURCE][WARNING]: A texture with the same id already existed so it was deleted! Id: " << id << "\n";
		delete _meshes[id];
	}

	MeshResource* meshResource = new MeshResource();
	meshResource->ID = id;
	meshResource->Vertices = vertices;
	meshResource->Indices = indices;
	Mesh* mesh = new Mesh(meshResource);

	_meshes[id] = mesh;
	return mesh;
}


// PRIVATE

Mesh* ResourceManager::CreateAndAddMesh(std::string id, std::string filename, std::string filepath) {
	if (_meshes.find(id) != _meshes.end()) {
		std::cout << "[RESOURCE][WARNING]: A model with the same id already existed so it was deleted! Id: " << id << "\n";
		delete _meshes[id];
	}

	ModelResource* modelResource = new ModelResource();
	modelResource->Filename = filename;
	modelResource->Path = filepath;
	modelResource->ID = id;
	Model* model = new Model(modelResource);

	Mesh* mesh = model->GetFirstMesh(id);
	
	delete model;

	_meshes[id] = mesh;
	
	return mesh;
}

void ResourceManager::CreateAndAddModel(std::string filename, std::string filepath, std::string id) {
	if (_models.find(id) != _models.end()) {
		std::cout << "[RESOURCE][WARNING]: A model with the same id already existed so it was deleted! Id: " << id << "\n";
		delete _models[id];
	}

	ModelResource* modelResource = new ModelResource();
	modelResource->Filename = filename;
	modelResource->Path = filepath;
	modelResource->ID = id;
	Model* model = new Model(modelResource);

	_models[id] = model;
}

void ResourceManager::CreateAndAddShader(std::string path, std::string filenameVS, std::string filenameFS, std::string id) {
	if (_shaders.find(id) != _shaders.end()) {
		std::cout << "[RESOURCE][WARNING]: A shader with the same id already existed so it was deleted! Id: " << id << "\n";
		delete _shaders[id];
	}
	
	ShaderResource* shaderResource = new ShaderResource();
	shaderResource->VertexShaderFilename = filenameVS;
	shaderResource->FragmentShaderFilename = filenameFS;
	shaderResource->Path = path;
	shaderResource->ID = id;
	Shader* shader = new Shader(shaderResource);

	_shaders[id] = shader;
}

