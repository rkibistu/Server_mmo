#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
 * Class used to define all callbacks registered to WindowObject.
 * 
 * We need this because all callbacks must be static. And static methods
 * can't acces non static members. So we created this class where each
 * method call a method from WindowObject using Engine singelton (WindowObject
 * is unique, so we always acces the same one)
 */
class WindowCallbacks {
private:
    WindowCallbacks() = delete;

public:
    // Window events
    static void OnClose(GLFWwindow* W);
    static void OnResize(GLFWwindow* W, int width, int height);
    static void OnError(int error, const char* description);

    // KeyBoard
    static void KeyCallback(GLFWwindow* W, int key, int scancode, int action, int mods);

    // Mouse
    static void CursorMove(GLFWwindow* W, double posX, double posY);
    static void MouseClick(GLFWwindow* W, int button, int action, int mods);
    static void MouseScroll(GLFWwindow* W, double offsetX, double offsetY);
};