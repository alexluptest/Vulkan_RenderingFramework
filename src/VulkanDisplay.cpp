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

void VulkanDisplay::cleanup(VkDevice device, VkInstance instance)
{
    if (m_swapChain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(device, m_swapChain, nullptr);

    if (m_surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(instance, m_surface, nullptr);
}

bool VulkanDisplay::querySwapchainSupport(VkPhysicalDevice physicalDevice)
{
    VkResult res = VK_SUCCESS;

    // Get physical device surface capabilities
    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &m_surfaceCapabilities);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get the physical device surface capabilities. \n";
        return false;
    }

    // Get physical device format support for the given surface
    uint32_t formatCount = 0;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get the format count supported by the current surface.\n";
        return false;
    }

    if (formatCount != 0)
    {
        m_surfaceFormats.resize(formatCount);
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, m_surfaceFormats.data());
        if (res != VK_SUCCESS)
        {
            std::cout << "Failed to get the formats supported by the current surface.\n";
            return false;
        }
    }

    // Get supported presentation modes
    uint32_t presentModeCount = 0;
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get the number of available presentation modes. \n";
        return false;
    }
    if (presentModeCount != 0)
    {
        m_presentModes.resize(presentModeCount);
        res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, m_presentModes.data());
        if (res != VK_SUCCESS)
        {
            std::cout << "Failed to get the available presentation modes. \n";
            return false;
        }
    }

    // Success
    return true;
}

bool VulkanDisplay::initSwapchain(const VulkanPhysicalDevice &physicalDevice, 
    const VulkanLogicalDevice &logicalDevice, 
    const VulkanDisplay &display, 
    uint32_t width, uint32_t height)
{
    bool res = false;
    
    // Query swap chain support
    res = querySwapchainSupport(physicalDevice.get());
    if (res == false)
        return res;

    VkSurfaceFormatKHR surfaceFormat = chooseSwapchainFormat();
    VkPresentModeKHR presentMode = choosePresentMode();
    VkExtent2D swapChainExtent = chooseSwapchainExtent(width, height);
    uint32_t imageCount = chooseSwapchainImageCount();

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = display.surface();
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = swapChainExtent;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.minImageCount = imageCount;
    
    int graphicsQueueFamilyIndex = physicalDevice.getGraphicsQueueFamilyIndex();
    int presentQueueFamilyIndex = physicalDevice.getPresentationQueueFamilyIndex();
    uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(graphicsQueueFamilyIndex), static_cast<uint32_t>(presentQueueFamilyIndex) }; 
    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
    {
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    }
    else
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.preTransform = m_surfaceCapabilities.currentTransform;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // Create the swap chain
    if (vkCreateSwapchainKHR(logicalDevice.get(), &swapChainCreateInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        std::cout << "Failed to create swap chain. \n";
        return false;
    }

    // Success
    return true;
}

VkSurfaceFormatKHR VulkanDisplay::chooseSwapchainFormat()
{
    bool surfaceFormatFound = false;

    // Choose a surface format and a color space
    VkSurfaceFormatKHR surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    // Check if the chose color space and surface format is available
    for (auto &format : m_surfaceFormats)
    {
        if (format.format == surfaceFormat.format && format.colorSpace == surfaceFormat.colorSpace)
        {
            surfaceFormatFound = true;
            break;
        }
    }

    // Fallback to the first available surface format and color space
    if (surfaceFormatFound == false)
    {
        if (m_surfaceFormats.size() != 0)
        {
            surfaceFormat.format = m_surfaceFormats[0].format;
            surfaceFormat.colorSpace = m_surfaceFormats[0].colorSpace;
        }
        else
            std::cout << "No available surface formats to iterate. \n";
    }

    return surfaceFormat;
}

VkPresentModeKHR VulkanDisplay::choosePresentMode()
{
    bool presentModeFound = false;
    
    // Choose a present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Check if the chose present mode is available
    for (auto mode : m_presentModes)
    {
        if (mode == presentMode)
        {
            presentModeFound = true;
            break;
        }
    }

    // Fallback to the first available option
    if (presentModeFound == false)
    {
        if (m_presentModes.size() != 0)
            presentMode = m_presentModes[0];
        else
            std::cout << "No available present modes to choose from. \n";
    }

    return presentMode;
}

 VkExtent2D VulkanDisplay::chooseSwapchainExtent(uint32_t width, uint32_t height)
 {
    VkExtent2D swapChainExtent = { width, height };

    // Clamp the width and the height to the minimum and maximum extent reported by the surface capabilities
    width = std::min(std::max(m_surfaceCapabilities.minImageExtent.width, width), m_surfaceCapabilities.maxImageExtent.width);
    height = std::min(std::max(m_surfaceCapabilities.minImageExtent.height, height), m_surfaceCapabilities.maxImageExtent.height);

    return swapChainExtent;
 }

 uint32_t VulkanDisplay::chooseSwapchainImageCount()
 {
    if (m_bufferCount < m_surfaceCapabilities.minImageCount)
        m_bufferCount = m_surfaceCapabilities.minImageCount;

    // No limit for the swapchain image count
    if (m_surfaceCapabilities.maxImageCount != 0)
    {
        if (m_bufferCount > m_surfaceCapabilities.maxImageCount)
            m_bufferCount = m_surfaceCapabilities.maxImageCount;
    }

    return m_bufferCount;
 }