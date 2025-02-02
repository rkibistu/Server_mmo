#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "WindowObject.h"
#include <core/Engine.h>
#include <utils/Utils.h>

WindowObject::WindowObject(rml::Vector2 resolution)
	:_resolution(resolution), _cursorPosition(resolution / 2), _resizeEvent(false) {

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	_handler = glfwCreateWindow(_resolution.x, _resolution.y, "LearnOpenGL", NULL, NULL);
	if (_handler == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(_handler);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Enable culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Set default state
	_mouseButtonAction = 0;
	_mouseButtonStates = 0;
	_registeredKeyEvents = 0;
	memset(_keyStates, 0, 384);

	SetWindowCallbakcs();
}

void WindowObject::PollEvents() {
	glfwPollEvents();
}

void WindowObject::SwapBuffers() {
	glfwSwapBuffers(_handler);
}

void WindowObject::Close() {
	glfwSetWindowShouldClose(_handler, 1);
}

int WindowObject::ShouldClose() {
	return glfwWindowShouldClose(_handler);
}

void WindowObject::UpdateObservers() {
	ComputeFrameTime();

	// Signal window resize
	if (_resizeEvent) {
		_resizeEvent = false;
		for (auto obs : _observers) {
			obs->OnWindowResize(_resolution.x, _resolution.y);
		}
	}

	// Signal mouse move event
	if (_mouseMoveEvent) {
		_mouseMoveEvent = false;
		for (auto obs : _observers) {
			obs->OnMouseMove(_cursorPosition.x, _cursorPosition.y, _mouseDeltaX, _mouseDeltaY);
		}
	}

	// Signal mouse button press event
	auto pressEvent = _mouseButtonAction & _mouseButtonStates;
	if (pressEvent) {
		for (auto obs : _observers) {
			obs->OnMouseBtnPress(_cursorPosition.x, _cursorPosition.y, pressEvent, _keyMods);
		}
	}

	// Signal mouse button release event
	auto releaseEvent = _mouseButtonAction & (~_mouseButtonStates);
	if (releaseEvent) {
		for (auto obs : _observers) {
			obs->OnMouseBtnRelease(_cursorPosition.x, _cursorPosition.y, releaseEvent, _keyMods);
		}
	}

	// Signal mouse scroll event
	if (_scrollEvent) {
		_scrollEvent = false;
		for (auto obs : _observers) {
			obs->OnMouseScroll(_cursorPosition.x, _cursorPosition.y, _mouseScrollDeltaX, _mouseScrollDeltaY);
		}
	}

	// Signal key events
	if (_registeredKeyEvents) {
		for (int i = 0; i < _registeredKeyEvents; i++) {
			for (auto obs : _observers)
				_keyStates[_keyEvents[i]] ? obs->OnKeyPress(_keyEvents[i], _keyMods) : obs->OnKeyRelease(_keyEvents[i], _keyMods);
		}
		_registeredKeyEvents = 0;
	}

	// Continuous events
	for (auto obs : _observers) {
		obs->OnInputUpdate(static_cast<float>(deltaFrameTime), _keyMods);
	}

	_mouseButtonAction = 0;
}

bool WindowObject::KeyHold(int keyCode) const {
	return _keyStates[keyCode];
}

bool WindowObject::MouseHold(int button) const {
	return IS_BIT_SET(_mouseButtonStates, button);
}


int WindowObject::GetSpecialKeyState() const {
	return _keyMods;
}

void WindowObject::SetSize(int width, int height) {
	glfwSetWindowSize(_handler, width, height);
	_resolution.x = width;
	_resolution.x = height;

	_resizeEvent = true;
}

// PRIVATE

void WindowObject::ComputeFrameTime() {
	frameID++;
	double currentTime = Engine::GetElapsedTime();
	deltaFrameTime = currentTime - elapsedTime;
	elapsedTime = currentTime;
}

void WindowObject::SetWindowCallbakcs() {
	// We need a middleware class (windowcallbacks) because
	// callbacks have to be static methods and from static emthods
	// we can't acces not static members
	glfwSetKeyCallback(_handler, WindowCallbacks::KeyCallback);
	glfwSetMouseButtonCallback(_handler, WindowCallbacks::MouseClick);
	glfwSetCursorPosCallback(_handler, WindowCallbacks::CursorMove);
	glfwSetScrollCallback(_handler, WindowCallbacks::MouseScroll);
	glfwSetWindowCloseCallback(_handler, WindowCallbacks::OnClose);
	glfwSetWindowSizeCallback(_handler, WindowCallbacks::OnResize);
}

void WindowObject::KeyCallback(int key, int scancode, int action, int mods) {
	_keyMods = mods;
	if (_keyStates[key] == (action ? true : false))
		return;
	_keyStates[key] = action ? true : false;
	_keyEvents[_registeredKeyEvents] = key;
	_registeredKeyEvents++;
}

void WindowObject::MouseButtonCallback(int button, int action, int mods) {
	// Only button events and mods are kept
	// Mouse position is the current frame position
	_keyMods = mods;
	SET_BIT(_mouseButtonAction, button);
	action ? SET_BIT(_mouseButtonStates, button) : CLEAR_BIT(_mouseButtonStates, button);
}

void WindowObject::MouseMove(int posX, int posY) {

	// Save information for processing later on the Update thread
	if (_mouseMoveEvent) {
		_mouseDeltaX += posX - _cursorPosition.x;
		_mouseDeltaY += posY - _cursorPosition.y;
	}
	else {
		_mouseMoveEvent = true;
		_mouseDeltaX = posX - _cursorPosition.x;
		_mouseDeltaY = posY - _cursorPosition.y;
	}
	_cursorPosition = rml::Vector2(posX, posY);
}

void WindowObject::MouseScroll(double offsetX, double offsetY) {
	_mouseScrollDeltaX = (int)offsetX;
	_mouseScrollDeltaY = (int)offsetY;

	_scrollEvent = true;
}

void WindowObject::SubscribeToEvents(InputController* inputController) {
	_observers.push_back(inputController);
}

void WindowObject::UnsubscribeFromEvents(InputController* inputController) {
	_observers.remove(inputController);
}
