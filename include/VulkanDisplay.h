#ifndef VULKANDISPLAY_H
#define VULKANDISPLAY_H

#include "VulkanHelper.h"

class VulkanDisplay
{

public:

    VulkanDisplay() {}
    ~VulkanDisplay() {}

    bool createSurface(VkInstance instance, GLFWwindow *window);
    void cleanup(VkInstance instance);

    inline const VkSurfaceKHR surface() const { return m_surface; }

private:

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

};

#endif // VULKANDISPLAY_H