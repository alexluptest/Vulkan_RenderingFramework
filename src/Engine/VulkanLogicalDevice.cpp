#include "VulkanLogicalDevice.h"

#include <vector>
#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

bool VulkanLogicalDevice::init(VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex, 
    uint32_t queueCount)
{
    VkResult res = VK_SUCCESS;

    assert(queueFamilyIndex != -1 && "Invalid queue family index. \n");
    assert(queueCount != 0 && "Invalid queue count. Queue count needs to be >= 1. \n");

    // Device queue
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = queueCount;
    queueCreateInfo.flags = 0;
    // Equal queue priorities
    std::vector<float> queuePriorities;
    for (int i = 0; i < queueCount; i++)
        queuePriorities.push_back(1.0f);
    queueCreateInfo.pQueuePriorities = queuePriorities.data();

    // Logical device
    VkPhysicalDeviceFeatures requiredDeviceFeatures = {};
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pEnabledFeatures = &requiredDeviceFeatures;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    
    // Get device extensions
    uint32_t extensionCount = 0;
    res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get device extension count. \n";
        return false;
    }

    if (extensionCount != 0)
    {
        m_supportedDeviceExtensions.resize(extensionCount);
        res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, m_supportedDeviceExtensions.data());
        if (res != VK_SUCCESS)
        {
            std::cout << "Failed to enumerate device extensions. \n";
            return false;
        }

        // Print device extensions
        std::cout << "\nAvailable device extensions: \n";
        for (auto &deviceExtension : m_supportedDeviceExtensions)
            std::cout << deviceExtension.extensionName << "\n";
    }

    // Check device specific extensions
    std::vector<const char*> requiredDeviceExtensions;
    requiredDeviceExtensions = { "VK_KHR_swapchain" };
    if (checkDeviceExtensionSupport(requiredDeviceExtensions) == false)
    {
        std::cout << "Failed to find all the required device extensions. \n";
        return false;
    }
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
    deviceCreateInfo.enabledExtensionCount = requiredDeviceExtensions.size();

    // Create logical device
    res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &m_logicalDevice);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to create vulkan logical device. \n";
        return false;
    }

    // Success
    return true;
}

void VulkanLogicalDevice::cleanup()
{
    if (m_logicalDevice != VK_NULL_HANDLE)
        vkDestroyDevice(m_logicalDevice, nullptr);
}

bool VulkanLogicalDevice::checkDeviceExtensionSupport(const std::vector<const char*> &requiredDeviceExtensions)
{
    for (const auto &currentDeviceExtension : requiredDeviceExtensions)
    {
        bool extensionFound = false;

        for (const auto &deviceExtension : m_supportedDeviceExtensions)
        {
            if (strcmp(deviceExtension.extensionName, currentDeviceExtension) == 0)
            {
                extensionFound = true;
                break;
            }
        }

        if (!extensionFound)
            return false;
    }

    // Success
    return true;
}