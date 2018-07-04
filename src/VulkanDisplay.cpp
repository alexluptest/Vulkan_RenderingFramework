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

bool VulkanDisplay::initSwapchain(VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height)
{
    bool res = false;
    
    // Query swap chain support
    res = querySwapchainSupport(physicalDevice);
    if (res == false)
        return res;

    VkSurfaceFormatKHR surfaceFormat = chooseSwapchainFormat();
    VkPresentModeKHR presentMode = choosePresentMode();

    // Choose swapchain extent
    VkExtent2D swapChainExtent = chooseSwapchainExtent(width, height);

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