#ifndef VULKANDISPLAY_H
#define VULKANDISPLAY_H

#include "VulkanHelper.h"

#include <vector>

class VulkanDisplay
{

public:

    VulkanDisplay() {}
    ~VulkanDisplay() {}

    bool createSurface(VkInstance instance, GLFWwindow *window);
    void cleanup(VkInstance instance);
    bool initSwapchain(VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height);

    inline const VkSurfaceKHR surface() const { return m_surface; }

private:

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

    VkSurfaceCapabilitiesKHR m_surfaceCapabilities = {};
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;

    VkSurfaceFormatKHR m_surfaceFormat;
    VkPresentModeKHR m_presentMode;

    bool querySwapchainSupport(VkPhysicalDevice physicalDevice);
    VkSurfaceFormatKHR chooseSwapchainFormat();
    VkPresentModeKHR choosePresentMode();
    VkExtent2D chooseSwapchainExtent(uint32_t width, uint32_t height);

};

#endif // VULKANDISPLAY_H