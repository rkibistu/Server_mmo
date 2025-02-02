#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <components/Vertex.h>
#include <algorithm> 

#include "World3.h"
#include <core/Renderer.h>
#include <utils/Utils.h>
#include <core/Engine.h>

Worlds::World3::World3() {
}

Worlds::World3::~World3() {

}

void Worlds::World3::Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath) {
	World::Init(resourceXmlFilepath, sceneXmlFilepath);

	PhysicsController::GetInstance();
}

void Worlds::World3::FrameStart() {
	World::FrameStart();

	DrawCoordinateSystem();
}

void Worlds::World3::Update(float deltaTimeSeconds) {
	World::Update(deltaTimeSeconds);

	_accumulatedTime += deltaTimeSeconds;

	if (!IsPaused())
		//Fix deltatime for physics
		while (_accumulatedTime >= c_physicsDeltaTime) {
			PhysicsController::GetInstance().Update(c_physicsDeltaTime);
			_accumulatedTime -= c_physicsDeltaTime;
		}

	Renderer::DrawBox(
		PhysicsController::GetInstance().GetBoxMinBound(),
		PhysicsController::GetInstance().GetBoxMaxBound(),
		rml::Vector3(0, 0, 0),
		GL_TRIANGLES,
		"4_cube_reversed",
		"3"
	);

	if (Engine::GetElapsedTime() > _printTicker) {

		std::cout << "FPS: " << static_cast<int>(1.0 / deltaTimeSeconds) << "  Count: " << PhysicsController::GetInstance().GetNumberOfObjects() << "\n";
		_printTicker += 3;
	}
}

void Worlds::World3::FrameEnd() {
	World::FrameEnd();
}

void Worlds::World3::Destroy() {
	World::Destroy();

	PhysicsController::GetInstance().DestroyInstance();
}

void Worlds::World3::OnKeyPress(int key, int mods) {
	if (key == GLFW_KEY_1) {

		PhysicsController::GetInstance().PopulateTheScene(100, 250, 500);
	}
	if (key == GLFW_KEY_2) {

		PhysicsController::GetInstance().PopulateTheScene(250, 500, 1000);
	}
	if (key == GLFW_KEY_3) {

		PhysicsController::GetInstance().PopulateTheScene(500, 1000, 2500);
	}
	if (key == GLFW_KEY_X) {

		PhysicsController::GetInstance().ClearTheScene();
	}

	if (key == GLFW_KEY_Q) {
		PhysicsController::GetInstance().EnforceMovement(rml::Vector3(15, 100, 15));
	}


	if (key == GLFW_KEY_I) {

		int count = 1;
		if (mods == 1)
			count = 20;

		for (int i = 0; i < count; i++) {

			PhysicsController::GetInstance().CreateAndAddSphereObject(
				"physics",
				"3_sphere",
				"3",
				PhysicsController::GetInstance().GetRandomPositionInsideBox(),
				rml::Vector3(0, 0, 0),
				rml::Vector3(1, 1, 1),
				nullptr
			);
		}
	}
	if (key == GLFW_KEY_O) {
		int count = 1;
		if (mods == 1)
			count = 20;

		for (int i = 0; i < count; i++) {

			PhysicsController::GetInstance().CreateAndAddBoxObject(
				"physics",
				"1",
				"3",
				PhysicsController::GetInstance().GetRandomPositionInsideBox(),
				rml::Vector3(0, 0, 0),
				rml::Vector3(1, 1, 1),
				nullptr
			);
		}
	}
	if (key == GLFW_KEY_P) {
		int count = 1;
		if (mods == 1)
			count = 20;

		for (int i = 0; i < count; i++) {

			PhysicsController::GetInstance().CreateAndAddBoxObject(
				"physics",
				"6_cone",
				"3",
				PhysicsController::GetInstance().GetRandomPositionInsideBox(),
				rml::Vector3(0, 0, 0),
				rml::Vector3(1, 1, 1),
				nullptr
			);
		}
	}
}




