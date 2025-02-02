#pragma once

#include <string>

#define TEXTURE_TYPE_2D		"2d"

/**
 * Structure that represents the data found in resources.xml. This data is used to find
 * the texture filepath and other characteristics.
 */
struct TextureResource {

	std::string ID;
	std::string Path;
	std::string Filename;

	std::string FilePath() { return Path + Filename; }

	std::string Type;

	std::string MinFilter;
	std::string MagFilter;
	std::string WrapS;
	std::string WrapT;
};

/**
 * It is a RESOURCE that represents a texture.
 * Used to manage texture in OpenGL.
 */
class Texture {
	friend class ResourceManager;

private:
	// Private constructors so only ResourceManager can create resources 
	Texture() = delete;
	Texture(TextureResource* textureResource);
	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;
public:
	~Texture();

	int Load();

	void Bind(int slot);
	void Unbind();

	bool IsLoaded() { return _loaded; }

	inline std::string GetID() { return _textureResource->ID; }

private:	
	int LoadTexture2D();

	//Conversion from string to GL_VALUES
	unsigned int GetParam(std::string param);
private:
	TextureResource* _textureResource = nullptr;

	// True -> textureId was generated (fiels read, etc
	bool _loaded = false;
	unsigned int _textureId;

	int _width;
	int _height;
	int _nrChannels;
	unsigned char* _data = nullptr;
};
