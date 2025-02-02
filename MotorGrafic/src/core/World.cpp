#include "World.h"
#include "Engine.h"

#include <components/SceneObject.h>
#include "ResourceManager.h"
#include "SceneManager.h"
#include <components/CameraInput.h>

#include "MemoryDebug.h"


World::World() {

	_previousTime = 0;
	_elapsedTime = 0;
	_deltaTime = 0;
	_paused = false;
	_shouldClose = false;

	_window = Engine::GetWindow();
}

World::~World() {



}

void World::Init(std::string resourceXmlFilepath, std::string sceneXmlFilepath) {

	ResourceManager::GetInstance().ParseFile(resourceXmlFilepath);
	SceneManager::GetInstance().ParseFile(sceneXmlFilepath);

	_groundPlane = SceneManager::GetInstance().CreateUsingModel("test", "4_plane", "1", rml::Vector3(0, 0, 0), rml::Vector3(0, 0, 0), rml::Vector3(30, 1, 30), ObjectMovementType::SEMI_STATIC, nullptr);
	_line = SceneManager::GetInstance().CreateUsingMesh("test", "2_line", "color", rml::Vector3(0, 0, 0), rml::Vector3(0, 0, 0), rml::Vector3(2, 2, 2), ObjectMovementType::SEMI_STATIC, nullptr);

	_cameraInput = new CameraInput(SceneManager::GetInstance().GetActivecamera());
}

void World::FrameStart() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void World::Update(float deltaTimeSeconds) {
	std::unordered_map<unsigned int, iSceneObject*> sceneObjects = SceneManager::GetInstance().GetSceneObjects();
	for (auto it = sceneObjects.begin(); it != sceneObjects.end(); it++) {
		it->second->Update(deltaTimeSeconds);
	}
}

void World::Draw() {
	if (Engine::HasGraphics() == false)
		return;


	std::vector<iSceneObject*> sceneObjects = SceneManager::GetInstance().GetSceneObejctsInsideFrustrumOctree(SceneManager::GetInstance().GetActivecamera());
	for (auto it = sceneObjects.begin(); it != sceneObjects.end(); it++) {
		(*it)->Draw();
	}

	std::vector<iSceneObject*> kinematicObjects = SceneManager::GetInstance().GetKinematicObjects();
	for (auto it = kinematicObjects.begin(); it != kinematicObjects.end(); it++) {
		(*it)->Draw();
	}
}

void World::FrameEnd() {
}

void World::Destroy() {
	ResourceManager::GetInstance().DestroyInstance();
	SceneManager::GetInstance().DestroyInstance();

	if (_line) {
		delete _line;
		_line = nullptr;
	}
	if (_groundPlane) {
		delete _groundPlane;
		_groundPlane = nullptr;
	}
	if (_cameraInput) {
		delete _cameraInput;
		_cameraInput = nullptr;
	}
}

void World::DrawCoordinateSystem(bool drawGround) {
	Camera* camera = SceneManager::GetInstance().GetActivecamera();
	DrawCoordinateSystem(camera->GetViewmatrix(), camera->GetProjectionMatrix(), drawGround);
}

void World::DrawCoordinateSystem(const rml::Matrix& viewMatrix, const rml::Matrix& projectionMaxtix, bool drawGround) {

	//ground plane
	if (drawGround) {
		_groundPlane->SetDrawMode(GL_TRIANGLES);
		_groundPlane->Draw();
	}

	glLineWidth(5);
	//oz
	_line->SetRotation(rml::Vector3(0, 0, 0));
	_line->SetDrawMode(GL_LINES);
	_line->GetShader()->Bind();
	_line->GetShader()->SetUniform3f("u_color", rml::Vector3(0, 0, 1));
	_line->Draw();

	//ox
	_line->SetRotation(rml::Vector3(0, -90, 0));
	_line->GetShader()->Bind();
	_line->GetShader()->SetUniform3f("u_color", rml::Vector3(1, 0, 0));
	_line->Draw();

	//oy
	_line->SetRotation(rml::Vector3(90, 0, 0));
	_line->GetShader()->Bind();
	_line->GetShader()->SetUniform3f("u_color", rml::Vector3(0, 1, 0));
	_line->Draw();

	glLineWidth(1);
}

void World::Run() {

	if (!_window)
		return;

	while (!_window->ShouldClose()) {
		LoopUpdate();
	}

	Exit();
}

void World::Pause() {
	_paused = !_paused;
}

void World::Exit() {
	Destroy();
	_shouldClose = true;
	_window->Close();
}

double World::GetLastFrameTime() {
	return _deltaTime;
}

void World::ComputeFrameDeltaTime() {
	_elapsedTime = Engine::GetElapsedTime();
	_deltaTime = _elapsedTime - _previousTime;
	_previousTime = _elapsedTime;
}

void World::LoopUpdate() {

	if (Engine::HasGraphics()) {
		// Polls the events
		_window->PollEvents();
	}


	// Compute delta time
	ComputeFrameDeltaTime();

	// Calls the methods of the instance of InputController in the following order
	// OnWindowResize, OnMouseMove, OnMouseBtnPress, OnMouseBtnRelease, OnMouseScroll, OnKeyPress, OnMouseScroll, OnInputUpdate
	// OnInputUpdate will be called each frame, the other functions are called only if an event is registered
	if (Engine::HasGraphics()) {
		_window->UpdateObservers();
	}

	//Frame processing
	FrameStart();
	Update(static_cast<float>(_deltaTime));
	if (Engine::HasGraphics()) {
		Draw();
	}
	FrameEnd();

	//Display image on screen
	if (Engine::HasGraphics()) {
		_window->SwapBuffers();
	}
}
