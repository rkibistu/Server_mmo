#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <components/Vertex.h>

#include "World5.h"

Worlds::World5::World5() {

}

Worlds::World5::~World5() {

}

void Worlds::World5::Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath) {
	World::Init(resourceXmlFilepath, sceneXmlFilepath);

}

void Worlds::World5::FrameStart() {
	World::FrameStart(); 

	//DrawCoordinateSystem();
}

void Worlds::World5::Update(float deltaTimeSeconds) {
	//World::Update(deltaTimeSeconds);

}

void Worlds::World5::Draw() {
	//World::Draw();
}

void Worlds::World5::FrameEnd() {
	World::FrameEnd();
}

void Worlds::World5::Destroy() {
	World::Destroy();
}

void Worlds::World5::OnKeyPress(int key, int mods) {
	std::cout << "Key PRESSED: " << key << '\n';
}

void Worlds::World5::OnKeyRelease(int key, int mods) {
}

void Worlds::World5::OnInputUpdate(float deltaTime, int mods) {
}

void Worlds::World5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
}

void Worlds::World5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
}

void Worlds::World5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}

void Worlds::World5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}

void Worlds::World5::OnWindowResize(int width, int height) {
}

