#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <utils/TGA.h>
#include <core/Defines.h>
#include "Texture.h"

#include <core/MemoryDebug.h>

Texture::Texture(TextureResource* textureResource)
	: _textureResource(textureResource) {
	glGenTextures(1, &_textureId);
}

Texture::~Texture() {
	glDeleteTextures(1, &_textureId);

	if (_textureResource)
		delete _textureResource;
}

int Texture::Load() {
	if (_textureResource == nullptr) {
		std::cout << "[MESH][ERROR]: model resource is null!\n";
		return MY_ERROR_CODE;
	}

	_data = stbi_load(_textureResource->FilePath().c_str(), &_width, &_height, &_nrChannels, 0);
	if (_data == nullptr) {
		std::cout << "[TEXTURE][ERROR]: Failed to read data from file: " << _textureResource->FilePath() <<"\n";
		return MY_ERROR_CODE;
	}

	if(_textureResource->Type == TEXTURE_TYPE_2D) {
		LoadTexture2D();
	}
	else {
		std::cout << "[TEXTURE][ERROR]: Load texture, uknow type: " << _textureResource->Type << " \n";
		return MY_ERROR_CODE;
	}

	stbi_image_free(_data);

	_loaded = true;
	return MY_SUCCES_CODE;
}

void Texture::Bind(int slot) {
	if (!_loaded) {
		std::cout << "[TEXTURE][ERROR]: Can't bind a texture that is not loaded. Call Load method before. TextureID: " << _textureResource->ID << "\n";
		return;
	}
	glActiveTexture(GL_TEXTURE0 + slot);

	if (_textureResource->Type == TEXTURE_TYPE_2D) {

		glBindTexture(GL_TEXTURE_2D, _textureId);
	}
	else {
		std::cout << "[TEXTURE][ERROR]: Can't bind texture, unknow type: " << _textureResource->Type << " \n";
	}
}

void Texture::Unbind() {

	if (_textureResource->Type == TEXTURE_TYPE_2D) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

// PRIVATE

int Texture::LoadTexture2D() {
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetParam(_textureResource->MinFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetParam(_textureResource->MagFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetParam(_textureResource->WrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetParam(_textureResource->WrapT));

	unsigned int format;
	if (_nrChannels == 1)
		format = GL_RED;         // Grayscale image
	else if (_nrChannels == 3)
		format = GL_RGB;         // RGB image
	else if (_nrChannels == 4)
		format = GL_RGBA;
	else {
		std::cout << "[ERROR] Unknow number of channels value in texture: " << _nrChannels << std::endl;
		return MY_ERROR_CODE;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, _data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return MY_SUCCES_CODE;
}

unsigned int Texture::GetParam(std::string param) {
	if (param == "NEAREST")
		return GL_NEAREST;
	else if (param == "LINEAR")
		return GL_LINEAR;
	else if (param == "GL_REPEAT")
		return GL_REPEAT;
	else if (param == "CLAMP_TO_EDGE")
		return GL_CLAMP_TO_EDGE;
	else
		std::cout << "[TEXTURE][ERROR] Wrong texture param type: " << param << '\n';
}
