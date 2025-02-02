#pragma once

#include <core/World.h>
#include <components/Mesh.h>
#include <components/Shader.h>
#include <components/Texture.h>
#include <components/SceneObject.h>
#include <core/ResourceManager.h>
#include <core/SceneManager.h>
#include <physics/PhysicsController.h>
#include <components/CameraInput.h>

namespace Worlds {

	class World3 : public World {

	public:
		World3();
		~World3();

	private:
		void Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath) override;
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;
		void Destroy() override;

		virtual void OnKeyPress(int key, int mods) override;
	private:

		CameraInput* _cameraInput;

		const float c_physicsDeltaTime = 1.0f / 60.0f;
		float _accumulatedTime = 0.0f;

		int _printTicker = 3;
	};
}
