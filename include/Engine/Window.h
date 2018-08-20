#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:

    bool init(const std::string &title, uint16_t width, uint16_t height);
    void cleanup();

    inline GLFWwindow *window() const { return m_window; }

private:

    GLFWwindow *m_window = nullptr;

    static void error_callback(int error, const char* description);
};

#endif // WINDOW_H