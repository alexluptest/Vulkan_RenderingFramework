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
    bool createFramebuffers(VkDevice device, VkRenderPass renderPass);
    void cleanup(VkDevice device, VkInstance instance);
    bool initSwapchain(const VulkanPhysicalDevice &physicalDevice, 
        const VulkanLogicalDevice &logicalDevice,
        uint32_t width, uint32_t height);

    inline const VkSwapchainKHR swapChain() const { return m_swapChain; }
    inline const VkSurfaceKHR surface() const { return m_surface; }
    inline uint32_t imageCount() const { return m_bufferCount; }
    inline const VkSurfaceFormatKHR surfaceFormat() const { return m_surfaceFormat; }
    inline const VkPresentModeKHR presentMode() const { return m_presentMode; }
    inline const VkExtent2D surfaceExtent() const { return m_surfaceExtent; }
    inline const VkFramebuffer framebuffer(uint32_t framebufferIndex) const { return m_framebuffers[framebufferIndex]; }

private:

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    uint32_t m_bufferCount = 2;

    VkSurfaceCapabilitiesKHR m_surfaceCapabilities = {};
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;

    VkSurfaceFormatKHR m_surfaceFormat;
    VkPresentModeKHR m_presentMode;
    VkExtent2D m_surfaceExtent;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    std::vector<VkFramebuffer> m_framebuffers;

    bool querySwapchainSupport(VkPhysicalDevice physicalDevice);
    VkSurfaceFormatKHR chooseSwapchainFormat();
    VkPresentModeKHR choosePresentMode();
    VkExtent2D chooseSwapchainExtent(uint32_t width, uint32_t height);
    uint32_t chooseSwapchainImageCount();
    void createImageViews();

};

#endif // VULKANDISPLAY_H