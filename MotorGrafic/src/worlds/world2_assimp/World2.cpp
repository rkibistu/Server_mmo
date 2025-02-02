#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <components/Vertex.h>

#include "World2.h"

Worlds::World2::World2() {



}

Worlds::World2::~World2() {

	
}

void Worlds::World2::Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath) {
	World::Init(resourceXmlFilepath, sceneXmlFilepath);

}

void Worlds::World2::FrameStart() {
	World::FrameStart();

	DrawCoordinateSystem(true);
}

void Worlds::World2::Update(float deltaTimeSeconds) {
	World::Update(deltaTimeSeconds);
}

void Worlds::World2::FrameEnd() {
	World::FrameEnd();
}

void Worlds::World2::Destroy() {
	World::Destroy();

}

void Worlds::World2::OnKeyPress(int key, int mods) {
	std::cout << "Key PRESSED: " << key << '\n';
}

void Worlds::World2::OnKeyRelease(int key, int mods) {
	std::cout << "Key RELEASED: " << key << '\n';
}

void Worlds::World2::OnInputUpdate(float deltaTime, int mods) {

}

void Worlds::World2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
	//std::cout << "Mouse MOVED: " << mouseX << " " << mouseY << "\n";
}

void Worlds::World2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
	std::cout << "Mouse PRESSED: " << mouseX << " " << mouseY << " " << button << "\n";
}

void Worlds::World2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
	std::cout << "Mouse RELEASED: " << mouseX << " " << mouseY << " " << button << "\n";
}

void Worlds::World2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	std::cout << "Mouse SCROLL: " << mouseX << " " << mouseY << " " << offsetX << " " << offsetY  << "\n";
}

void Worlds::World2::OnWindowResize(int width, int height) {
	std::cout << "Window RESIZE: " << width << " " << height << '\n';
}

