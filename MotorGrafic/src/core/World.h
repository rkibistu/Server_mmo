#pragma once

#include "window/WindowObject.h"
#include <iostream>

class CameraInput;
class Frustrum;
class iSceneObject;

/**
 * Main class that is used to add functionality to the loop.
 *
 * This class should be inherited to create customised scenes.
 */
class World : public InputController {

public:
	World();
	virtual ~World();

	// Methods that are gonna be called in LoopUpdate
	virtual void Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath);
	virtual void FrameStart();
	virtual void Update(float deltaTimeSeconds);
	virtual void Draw();
	virtual void FrameEnd();
	virtual void Destroy();

	void DrawCoordinateSystem(bool drawGround = true);
	void DrawCoordinateSystem(const rml::Matrix& viewMatrix, const rml::Matrix& projectionMaxtix, bool drawGround = true);

	// Methods that dictate the flow of the program
	void Run();
	void Pause();
	void Exit();

	double GetLastFrameTime();
	inline bool IsPaused() { return _paused; }

protected:
	WindowObject* _window;
	CameraInput* _cameraInput;

private:
	void ComputeFrameDeltaTime();
	void LoopUpdate();

private:
	double _previousTime;
	double _elapsedTime;
	double _deltaTime;
	bool _shouldClose;
	bool _paused;



	//Debug elements
	iSceneObject* _groundPlane = nullptr;
	iSceneObject* _line = nullptr;
};
