#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <components/Vertex.h>

#include "World4.h"

Worlds::World4::World4() {

}

Worlds::World4::~World4() {

}

void Worlds::World4::Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath) {
	World::Init(resourceXmlFilepath, sceneXmlFilepath);

}

void Worlds::World4::FrameStart() {
	World::FrameStart(); 

	//DrawCoordinateSystem();
}

void Worlds::World4::Update(float deltaTimeSeconds) {
	World::Update(deltaTimeSeconds);

	std::vector<iSceneObject*> sceneObjects = SceneManager::GetInstance().GetAllObjects();
	//std::vector<SceneObject*> sceneObjectsInsideFrustrum = SceneManager::GetInstance().GetSceneObejctsInsideFrustrum(SceneManager::GetInstance().GetCamera("FrustrumCamera"));
	std::vector<iSceneObject*> sceneObjectsInsideFrustrum = SceneManager::GetInstance().GetSceneObejctsInsideFrustrumOctree(SceneManager::GetInstance().GetCamera("FrustrumCamera"));

	Material* redMaterial = ResourceManager::GetInstance().GetMaterial("red");
	Material* greenMaterial = ResourceManager::GetInstance().GetMaterial("green");
	for (auto& obj : sceneObjects) {
		obj->SetMaterial(redMaterial, 0, true);
	}
	for (auto& obj : sceneObjectsInsideFrustrum) {
		obj->SetMaterial(greenMaterial, 0, true);
	}
}

void Worlds::World4::Draw() {
	//Don t call draw from parent because we don t want to sue normal culling. We want to create a scenario to better see the culling results
	//World::Draw();

	// Draw all objects and change material of the one isnide the frustrum (did this in update)
	// This helps to see the effects of frustruma nd octree better
	std::vector<iSceneObject*> sceneObjects = SceneManager::GetInstance().GetAllObjects();
	for (auto it = sceneObjects.begin(); it != sceneObjects.end(); it++) {
		(*it)->Draw();
	}

	SceneManager::GetInstance().GetCamera("FrustrumCamera")->DrawFrustrum();
	SceneManager::GetInstance().OctreeDebugVisual();
}

void Worlds::World4::FrameEnd() {
	World::FrameEnd();
}

void Worlds::World4::Destroy() {
	World::Destroy();
}

void Worlds::World4::OnKeyPress(int key, int mods) {
	std::cout << "Key PRESSED: " << key << '\n';

	if (key == GLFW_KEY_G) {
		SceneManager::GetInstance().CreateOctree(rml::Vector3(-10, -10, -10), rml::Vector3(10, 10, 10), 3);
	}

	if (key == GLFW_KEY_0)
		_planeSelected = 0;
	if (key == GLFW_KEY_1)
		_planeSelected = 1;
	if (key == GLFW_KEY_2)
		_planeSelected = 2;
	if (key == GLFW_KEY_3)
		_planeSelected = 3;
	if (key == GLFW_KEY_4)
		_planeSelected = 4;
	if (key == GLFW_KEY_5)
		_planeSelected = 5;
}

void Worlds::World4::OnKeyRelease(int key, int mods) {
}

void Worlds::World4::OnInputUpdate(float deltaTime, int mods) {

	Camera* frustrumCamera = SceneManager::GetInstance().GetCamera("FrustrumCamera");
	if (_window->KeyHold(GLFW_KEY_I))            frustrumCamera->MoveForward(deltaTime);
	if (_window->KeyHold(GLFW_KEY_K))            frustrumCamera->MoveBackward(deltaTime);
	if (_window->KeyHold(GLFW_KEY_J))            frustrumCamera->MoveLeft(deltaTime);
	if (_window->KeyHold(GLFW_KEY_L))            frustrumCamera->MoveRight(deltaTime);
	if (_window->KeyHold(GLFW_KEY_U))            frustrumCamera->MoveDown(deltaTime);
	if (_window->KeyHold(GLFW_KEY_O))            frustrumCamera->MoveUp(deltaTime);
}

void Worlds::World4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
}

void Worlds::World4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
}

void Worlds::World4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}

void Worlds::World4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}

void Worlds::World4::OnWindowResize(int width, int height) {
}

