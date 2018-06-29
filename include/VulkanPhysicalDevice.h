#ifndef VULKANPHYSICALDEVICE_H
#define VULKANPHYSICALDEVICE_H

#include "VulkanHelper.h"

class VulkanPhysicalDevice
{

public:

    bool init(VkInstance instance);

private:

    struct DeviceRank
    {
        uint16_t deviceScore = 0;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    };
    static bool rankComparator(const DeviceRank &r1, const DeviceRank &r2) { return r1.deviceScore > r2.deviceScore; }

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    bool hasRequiredFeatures(VkPhysicalDevice &physicalDevice,
        VkPhysicalDeviceProperties &deviceProperties,
        VkPhysicalDeviceFeatures &deviceFeatures,
        uint16_t &deviceScore);

};

#endif // VULKANPHYSICALDEVICE_H