#include "VulkanPhysicalDevice.h"

#include <iostream>
#include <vector>
#include <algorithm>

bool VulkanPhysicalDevice::init(VkInstance instance)
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
            currenDeviceRank.physicalDevice = currentDevice;
            physicalDeviceRanks.push_back(currenDeviceRank);
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