#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Engine.h"

#include "MemoryDebug.h"

WindowObject* Engine::_window = nullptr;
bool Engine::_consoleMode = false;

WindowObject* Engine::Init(rml::Vector2 windowResolution) {
	if (!glfwInit())
		exit(0);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	_window = new WindowObject(windowResolution);

	return _window;
}

WindowObject* Engine::GetWindow() {
	return _window;
}


double Engine::GetElapsedTime() {
	return glfwGetTime();
}

void Engine::Exit() {;
	std::cout << "[ENGINE]: Engine closed. Exit.";
	if (_window)
		delete _window;
	glfwTerminate();
}
