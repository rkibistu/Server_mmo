#pragma once

#include <core/World.h>
#include <components/Mesh.h>
#include <components/Shader.h>
#include <components/Texture.h>
#include <components/SceneObject.h>
#include <core/ResourceManager.h>
#include <core/SceneManager.h>
#include <components/CameraInput.h>

namespace Worlds {

	class World2 : public World {

	public:
		World2();
		~World2();

	private:
		void Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath) override;
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;
		void Destroy() override;

		virtual void OnKeyPress(int key, int mods) override;

		virtual void OnKeyRelease(int key, int mods) override;

		virtual void OnInputUpdate(float deltaTime, int mods) override;

        virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

        virtual void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;

        virtual void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

        virtual void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        virtual void OnWindowResize(int width, int height) override;
	};
}
