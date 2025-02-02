#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//Used for checking heap memory

#include "src/utils/Math.h"
#include "src/core/Engine.h"
#include "src/core/World.h"
#include "src/worlds/world2_assimp/World2.h"
#include "src/worlds/world3_physics/World3.h"
#include "src/worlds/world4_octree/World4.h"
#include "src/worlds/world5_mmo/World5.h"

#include "src/test.h"

#include "windows.h"
#define _CRTDBG_MAP_ALLOC //to get more details

 int main(){
    //take a snapshot of the heap
    _CrtMemState sOld;
    _CrtMemState sNew;
    _CrtMemState sDiff;
    _CrtMemCheckpoint(&sOld); 

    Engine::Init(rml::Vector2(800, 600));

    World* world = new Worlds::World5();

    world->Init("resources/xmls/world5/resourceManager.xml", "resources/xmls/world5/sceneManager.xml");
    world->Run();

    delete world;
     
    Engine::Exit();

    // Take a snapshot of the heap
    _CrtMemCheckpoint(&sNew); 
    // Check for diffrences
    if (_CrtMemDifference(&sDiff, &sOld, &sNew)) 
    {
        OutputDebugString(L"-----------_CrtMemDumpStatistics ---------");
        _CrtMemDumpStatistics(&sDiff);
        OutputDebugString(L"-----------_CrtMemDumpAllObjectsSince ---------");
        _CrtMemDumpAllObjectsSince(&sOld);
        OutputDebugString(L"-----------_CrtDumpMemoryLeaks ---------");
        _CrtDumpMemoryLeaks();
    }

    return 0;
}
