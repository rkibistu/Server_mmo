#pragma once

#include <utils/Math.h>
#include "window/WindowObject.h"

class Engine {

public:
	static WindowObject* Init(rml::Vector2 windowResolution);

	static WindowObject* GetWindow();

	// Get elapsed time in seconds since the application started
	static double GetElapsedTime();

	static void Exit();

	static bool HasGraphics() { return _consoleMode == false; }

private:
	static WindowObject* _window;
	static bool _consoleMode;
};
