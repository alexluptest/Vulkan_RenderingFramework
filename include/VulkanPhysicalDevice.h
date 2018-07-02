#ifndef VULKANPHYSICALDEVICE_H
#define VULKANPHYSICALDEVICE_H

#include "VulkanHelper.h"

class VulkanPhysicalDevice
{

public:

    bool init(VkInstance instance);
    bool findQueueFamilies(VkQueueFlags requiredQueueFamilyFlags);

    inline const int getQueueFamilyIndex() const { return m_queueFamilyIndex; }
    inline const VkPhysicalDevice &get() const { return m_physicalDevice; }

private:

    struct DeviceRank
    {
        uint16_t deviceScore = 0;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    };
    static bool rankComparator(const DeviceRank &r1, const DeviceRank &r2) { return r1.deviceScore > r2.deviceScore; }

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    int m_queueFamilyIndex = -1;

    bool hasRequiredFeatures(VkPhysicalDevice &physicalDevice,
        VkPhysicalDeviceProperties &deviceProperties,
        VkPhysicalDeviceFeatures &deviceFeatures,
        uint16_t &deviceScore);
};

#endif // VULKANPHYSICALDEVICE_H