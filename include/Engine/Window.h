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

    inline GLFWwindow *get() const { return m_window; }
    inline const uint16_t width() const { return m_width; }
    inline const uint16_t height() const { return m_height; }

private:

    GLFWwindow *m_window = nullptr;
    uint16_t m_width, m_height;

    static void error_callback(int error, const char* description);
};

#endif // WINDOW_H