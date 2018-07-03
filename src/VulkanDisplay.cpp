#include "VulkanDisplay.h"

#include <assert.h>
#include <iostream>

bool VulkanDisplay::createSurface(VkInstance instance, GLFWwindow *window)
{
    assert(window && "Invalid window.");

    VkResult res = VK_SUCCESS;
    res = glfwCreateWindowSurface(instance, window, nullptr, &m_surface);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to create window surface. \n";
        return false;
    }

    // Success
    return true;
}

void VulkanDisplay::cleanup(VkInstance instance)
{
    if (m_surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(instance, m_surface, nullptr);
}