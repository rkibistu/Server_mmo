#include "InputController.h"
#include <core/Engine.h>

#include "MemoryDebug.h"

InputController::InputController() {
    Engine::GetWindow()->SubscribeToEvents(this);
}


InputController::~InputController() {
}
