#ifndef VULKANDISPLAY_H
#define VULKANDISPLAY_H

#include "VulkanHelper.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanDisplay.h"

#include <vector>

class VulkanDisplay
{

public:

    VulkanDisplay() {}
    ~VulkanDisplay() {}

    bool createSurface(VkInstance instance, GLFWwindow *window);
    void cleanup(VkDevice device, VkInstance instance);
    bool initSwapchain(const VulkanPhysicalDevice &physicalDevice, 
        const VulkanLogicalDevice &logicalDevice, 
        const VulkanDisplay &display, 
        uint32_t width, uint32_t height);

    inline const VkSurfaceKHR surface() const { return m_surface; }
    inline uint32_t imageCount() const { return m_bufferCount; }

private:

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    uint32_t m_bufferCount = 2;

    VkSurfaceCapabilitiesKHR m_surfaceCapabilities = {};
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;

    VkSurfaceFormatKHR m_surfaceFormat;
    VkPresentModeKHR m_presentMode;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;

    bool querySwapchainSupport(VkPhysicalDevice physicalDevice);
    VkSurfaceFormatKHR chooseSwapchainFormat();
    VkPresentModeKHR choosePresentMode();
    VkExtent2D chooseSwapchainExtent(uint32_t width, uint32_t height);
    uint32_t chooseSwapchainImageCount();

};

#endif // VULKANDISPLAY_H