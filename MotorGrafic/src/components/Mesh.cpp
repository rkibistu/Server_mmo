#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <utils/Math.h>
#include <core/Defines.h>
#include "Mesh.h"
#include "Vertex.h"

#include <core/MemoryDebug.h>

Mesh::Mesh(MeshResource* _meshResource)
	: _meshResource(_meshResource) {

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);
}
Mesh::~Mesh() {

	glDeleteBuffers(1, &_EBO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_VAO);

	if (_meshResource)
		delete _meshResource;
}


int Mesh::Load() {

	if (_meshResource == nullptr) {
		std::cout << "[MESH][ERROR]: model resource is null!\n";
		return MY_ERROR_CODE;
	}

	int res;

	//CreateWiredindicesBuffer(_meshResource->Indices, _meshResource->WiredIndices);

	BindAndLoadVertices();

	_loaded = true;
	return MY_SUCCES_CODE;
}

void Mesh::BindFilled() {
	if (!_loaded) {
		std::cout << "[MESH][ERROR]: Can't bind a mesh that is not loaded. Call Load method before. MeshID: " << _meshResource->ID << "\n";
		return;
	}
	glBindVertexArray(_VAO);
}
void Mesh::BindWired() {


}
void Mesh::Unbind() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::Draw(int drawMode) {
	glDrawElements(drawMode, GetIndicesFilledCount(), GL_UNSIGNED_INT, 0);
}

// PRIVATE

void Mesh::CreateWiredindicesBuffer(std::vector<unsigned int>& indices, std::vector<unsigned int>& wiredIndices) {

	wiredIndices.reserve(indices.size() * 2);

	for (unsigned int i = 0; i < indices.size(); i += 3) {
		unsigned int index1 = indices[i];
		unsigned int index2 = indices[i + 1];
		unsigned int index3 = indices[i + 2];

		wiredIndices.push_back(index1);
		wiredIndices.push_back(index2);
		wiredIndices.push_back(index2);
		wiredIndices.push_back(index3);
		wiredIndices.push_back(index3);
		wiredIndices.push_back(index1);
	}
}

void Mesh::BindAndLoadVertices() {
	//bind and load vertices buffer

	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, _meshResource->Vertices.size() * sizeof(Vertex), _meshResource->Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _meshResource->Indices.size() * sizeof(unsigned int), _meshResource->Indices.data(), GL_STATIC_DRAW);

	//TODO: this should be deal with somehow not hardcoded. Just testing now
	glVertexAttribPointer(VertexAttribLocation::POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(VertexAttribLocation::POS);
	glVertexAttribPointer(VertexAttribLocation::UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(VertexAttribLocation::UV);
	glVertexAttribPointer(VertexAttribLocation::NORM, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
	glEnableVertexAttribArray(VertexAttribLocation::NORM);
	glVertexAttribPointer(VertexAttribLocation::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(VertexAttribLocation::COLOR);
	glVertexAttribPointer(VertexAttribLocation::BINORM, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, binorm));
	glEnableVertexAttribArray(VertexAttribLocation::BINORM);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


