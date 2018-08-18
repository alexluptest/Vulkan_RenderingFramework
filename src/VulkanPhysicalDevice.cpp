#include "VulkanPhysicalDevice.h"

#include <iostream>
#include <vector>
#include <algorithm>

bool VulkanPhysicalDevice::init(VkInstance instance, 
    VkQueueFlags requiredQueueFamilyFlags, 
    VkSurfaceKHR surface)
{
    VkResult res;

    uint32_t deviceCount = 0;
    res = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get the physical device count. \n";
        return false;
    }

    // If the device count is 0 then there is no support for vulkan
    if (deviceCount == 0)
    {
        std::cout << "Failed to find a phyiscal device with vulkan support.\n";
        return false;
    }

    std::vector<VkPhysicalDevice> phyiscalDevices(deviceCount);
    res = vkEnumeratePhysicalDevices(instance, &deviceCount, phyiscalDevices.data());
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get the list of physical devices available. \n";
        return false;
    }

    // Check the physical device suitability based on the required features
    std::vector<DeviceRank> physicalDeviceRanks;
    for (auto &currentDevice : phyiscalDevices)
    {
        // Device name, type and supported Vulkan version
        VkPhysicalDeviceProperties deviceProperties = {};
        vkGetPhysicalDeviceProperties(currentDevice, &deviceProperties);

        // Texture compression, 64 bit floats, multi viewport rendering
        VkPhysicalDeviceFeatures deviceFeatures = {};
        vkGetPhysicalDeviceFeatures(currentDevice, &deviceFeatures);

        // Rate devices and check required features

        // Build a list of suitable devices along with their scores
        DeviceRank currenDeviceRank = {};
        if (hasRequiredFeatures(currentDevice, deviceProperties, deviceFeatures, currenDeviceRank.deviceScore))
        {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(currentDevice, &queueFamilyCount, nullptr);
            if (queueFamilyCount == 0)
            {
                std::cout << "No queue families found. \n";
                return false;
            }
            std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(currentDevice, &queueFamilyCount, queueFamilyProperties.data());

            // Only add devices that have presentation support
            for (auto &queueFamilyProperty : queueFamilyProperties)
            {
                int queueFamilyIndex = &queueFamilyProperty - &queueFamilyProperties[0];
                // Check if the current device has presentation support
                VkBool32 presentationSupport = VK_FALSE;
                res = vkGetPhysicalDeviceSurfaceSupportKHR(currentDevice, queueFamilyIndex, surface, &presentationSupport);
                if (res != VK_SUCCESS)
                    std::cout << "Failed to check if the current device has presentation support for the current queue family. \n";
                if (presentationSupport)
                {
                    // We found a queue family that has presentation support for the current device so we can stop
                    currenDeviceRank.physicalDevice = currentDevice;
                    physicalDeviceRanks.push_back(currenDeviceRank);
                    break;
                }
            }
        }
    }

    // Sort list of physical device by their score
    std::sort(physicalDeviceRanks.begin(), physicalDeviceRanks.end(), VulkanPhysicalDevice::rankComparator);

    // Print list of suitable device sorted by their scores
    std::cout << "\nList of physical devices sorted by their scores: \n";
    for (auto &currentDeviceRank : physicalDeviceRanks)
    {
        VkPhysicalDeviceProperties deviceProperties = {};
        vkGetPhysicalDeviceProperties(currentDeviceRank.physicalDevice, &deviceProperties);
        
        // Print device properties
        std::cout << "\nDevice name: " << deviceProperties.deviceName << "\n";
        std::cout << "API version: " << deviceProperties.apiVersion << "\n";
        std::cout << "Driver version: " << deviceProperties.driverVersion << "\n";
    }

    // Store a handle to the chosen physical device
    if (physicalDeviceRanks.size() != 0)
        m_physicalDevice = physicalDeviceRanks[0].physicalDevice;

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        std::cout << "Failed to find a suitable device that has all the required features. \n";
        return false;
    }

    // Get the memory properties supported by the chosen physical device
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);

    // Initialize queue family indices
    if (!findQueueFamilies(requiredQueueFamilyFlags, surface))
        return false;

    return true;
}

bool VulkanPhysicalDevice::hasRequiredFeatures(VkPhysicalDevice &physicalDevice,
    VkPhysicalDeviceProperties &deviceProperties,
    VkPhysicalDeviceFeatures &deviceFeatures,
    uint16_t &deviceScore)
{
    // Add specific features to be tested here

    // Calculate score
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        deviceScore += 5;
    else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        deviceScore += 3;
    else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ||
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
        deviceScore += 1;

    return true;
}

bool VulkanPhysicalDevice::findQueueFamilies(VkQueueFlags requiredQueueFamilyFlags, VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

    if (queueFamilyCount == 0)
    {
        std::cout << "No queue families found. \n";
        return false;
    }
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    // Print queue family properties
    std::cout << "\nQueue families: \n";
    for (auto &queueFamilyProp : queueFamilyProperties)
    {
        if (queueFamilyProp.queueCount > 0)
        {
            std::cout << "Queue count: " << queueFamilyProp.queueCount << "\n";
            if (queueFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                std::cout << "Graphics queue family. \n";
            if (queueFamilyProp.queueFlags & VK_QUEUE_COMPUTE_BIT)
                std::cout << "Compute queue familiy. \n";
            if (queueFamilyProp.queueFlags & VK_QUEUE_TRANSFER_BIT)
                std::cout << "Transfer queue family. \n";
            if (queueFamilyProp.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
                std::cout << "Sparse binding queue family. \n";
        }
    }

    for (auto &queueFamilyProp : queueFamilyProperties)
    {
        if (queueFamilyProp.queueCount > 0)
        {
            int queueFamilyIndex = &queueFamilyProp - &queueFamilyProperties[0];

            // Check if the current queue family has presentation support
            if (m_presentationQueueFamilyIndex == -1)
            {
                VkBool32 presentationSupport = VK_FALSE;
                VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, queueFamilyIndex, surface, &presentationSupport);
                if (res != VK_SUCCESS)
                    std::cout << "Failed to check if the current device has presentation support for the current queue family. \n";

                // Store queue family index that has presentation support
                if (presentationSupport)
                    m_presentationQueueFamilyIndex = queueFamilyIndex;
            }

            // Check is the current queue family supports graphics operations
            if (m_graphicsQueueFamilyIndex == -1)
            {
                if (requiredQueueFamilyFlags & VK_QUEUE_GRAPHICS_BIT) if ((queueFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) == false)
                    continue;
                if (requiredQueueFamilyFlags & VK_QUEUE_COMPUTE_BIT) if ((queueFamilyProp.queueFlags & VK_QUEUE_COMPUTE_BIT) == false)
                    continue;
                if (requiredQueueFamilyFlags & VK_QUEUE_TRANSFER_BIT) if ((queueFamilyProp.queueFlags & VK_QUEUE_TRANSFER_BIT) == false)
                    continue;
                if (requiredQueueFamilyFlags & VK_QUEUE_SPARSE_BINDING_BIT) if ((queueFamilyProp.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) == false)
                    continue;
                
                // Store queue family index that have all the required capabilities
                m_graphicsQueueFamilyIndex = &queueFamilyProp - &queueFamilyProperties[0];
            }

            if (m_presentationQueueFamilyIndex != -1 &&
                m_graphicsQueueFamilyIndex != -1)
                break;
        }
    }

    if (m_graphicsQueueFamilyIndex == -1)
    {
        std::cout << "Failed to find a queue family that satisfies all the requirements. \n";
        return false;
    }
    if (m_presentationQueueFamilyIndex == -1)
    {
        std::cout << "Failed to find a queue family that has presentation support. \n";
        return false;
    }

    return true;
}