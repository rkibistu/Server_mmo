#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Engine.h"

#include "MemoryDebug.h"

WindowObject* Engine::_window = nullptr;
bool Engine::_consoleMode = true;

WindowObject* Engine::Init(rml::Vector2 windowResolution) {
	if (!glfwInit())
		exit(0);

	if (_consoleMode == true)
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
