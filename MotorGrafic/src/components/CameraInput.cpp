#include <iostream>

#include "CameraInput.h"
#include <core/Engine.h>

#include <core/MemoryDebug.h>

CameraInput::CameraInput(Camera* camera)
	: _camera(camera) {
}

void CameraInput::OnInputUpdate(float deltaTime, int mods) {
    WindowObject* window = Engine::GetWindow();
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) return;

    if (window->GetSpecialKeyState() & GLFW_MOD_SHIFT) {
        deltaTime *= 2;
    }

    if (window->KeyHold(GLFW_KEY_W))            _camera->MoveForward(deltaTime);
    if (window->KeyHold(GLFW_KEY_S))            _camera->MoveBackward(deltaTime);
    if (window->KeyHold(GLFW_KEY_A))            _camera->MoveLeft(deltaTime);
    if (window->KeyHold(GLFW_KEY_D))            _camera->MoveRight(deltaTime);
    if (window->KeyHold(GLFW_KEY_Q))            _camera->MoveDown(deltaTime);
    if (window->KeyHold(GLFW_KEY_E))            _camera->MoveUp(deltaTime);

    // Here we multiply deltatime because the movement is too slow. But we don;t want to change
    // the rotation spped of camera because is going to be too fast for the case we use
    // mouse movement to rotate camera
    if (window->KeyHold(GLFW_KEY_KP_4))         _camera->RotateOY(200 *  deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_6))         _camera->RotateOY(200 * -deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_8))         _camera->RotateOX(200 * deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_2))         _camera->RotateOX(200 * -deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_7))         _camera->RotateOZ(200 * deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_9))         _camera->RotateOZ(200 * -deltaTime);
}

void CameraInput::OnKeyPress(int key, int mods) {
}

void CameraInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    WindowObject* window = Engine::GetWindow();
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        _camera->RotateOX(-(float)deltaY);
        _camera->RotateOY(-(float)deltaX);
    }
}

void CameraInput::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
}

void CameraInput::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}
