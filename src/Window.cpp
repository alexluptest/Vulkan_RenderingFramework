#include "Window.h"

#include <iostream>

bool Window::init(const std::string &title, uint16_t width, uint16_t height)
{
    glfwSetErrorCallback(Window::error_callback);

    if (glfwInit() == false)
    {
        std::cout << "Failed to initialize GLFW. \n";
        return false;
    }

    // Let GLFW know we don't need a OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Don't allow window to be resizable
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create window
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    // Success
    return true;
}

void Window::cleanup()
{
    if (m_window)
        glfwDestroyWindow(m_window);

    glfwTerminate();
}

void Window::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}