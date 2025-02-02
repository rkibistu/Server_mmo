#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <list>

#include <utils/Math.h>
#include "WindowCallbacks.h"
#include <core/InputController.h>

/**
 * Manages all operation with the window.
 * 
 * It manages event callbacks (close,resize,input, etc.)
 */
class WindowObject{
	friend class WindowCallbacks;
	friend class InputController;

public:
	WindowObject(rml::Vector2 resolution);

	void PollEvents();
	void SwapBuffers();
	void Close();
	int ShouldClose();

	// Update event listeners (key press / mouse move / window events)
	void UpdateObservers();

	// Get input state
	bool KeyHold(int keyCode) const;
	bool MouseHold(int button) const;
	int GetSpecialKeyState() const;

	// This needs to be static so it can be registered to glfw callbacks
	void SetSize(int width, int height);

	inline rml::Vector2 GetResolution() { return _resolution; }
private:
	// Frame time
	void ComputeFrameTime();

	void SetWindowCallbakcs();

	//WindowCallbacks
	void KeyCallback(int key, int scancode, int action, int mods);
	void MouseButtonCallback(int button, int action, int mods);
	void MouseMove(int posX, int posY);
	void MouseScroll(double offsetX, double offsetY);

	// Subscribe to receive input events
	void SubscribeToEvents(InputController* inputController);
	void UnsubscribeFromEvents(InputController* inputController);
private:
	GLFWwindow* _handler;
	rml::Vector2 _resolution;

	// Frame Time
	unsigned int frameID;
	double elapsedTime;
	double deltaFrameTime;

	// States for keyboard buttons - PRESSED(true) / RELEASED(false)
	int _registeredKeyEvents;
	int _keyEvents[128];
	bool _keyStates[384];

	// Special keys (ALT, CTRL, SHIFT, CAPS LOOK, OS KEY) active alongside with normal key or mouse input
	int _keyMods;
	
	// Mouse button callback
	int _mouseButtonCallback;            // bit field for button callback
	int _mouseButtonAction;              // bit field for button state
	int _mouseButtonStates;              // bit field for mouse button state

	// Mouse move event
	bool _mouseMoveEvent;
	int _mouseDeltaX;
	int _mouseDeltaY;
	rml::Vector2 _cursorPosition;

	// Mouse scroll event
	bool _scrollEvent;
	int _mouseScrollDeltaX;
	int _mouseScrollDeltaY;

	bool _resizeEvent;

	// Input Observers
	std::list<InputController*> _observers;
};
