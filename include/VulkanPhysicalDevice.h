#ifndef VULKANPHYSICALDEVICE_H
#define VULKANPHYSICALDEVICE_H

#include "VulkanHelper.h"

class VulkanPhysicalDevice
{

public:

    VulkanPhysicalDevice() {}
    ~VulkanPhysicalDevice() {}

    bool init(VkInstance instance, VkQueueFlags requiredQueueFamilyFlags, VkSurfaceKHR surface);

    inline const int getGraphicsQueueFamilyIndex() const { return m_graphicsQueueFamilyIndex; }
    inline const int getPresentationQueueFamilyIndex() const { return m_presentationQueueFamilyIndex; }
    inline const VkPhysicalDevice &get() const { return m_physicalDevice; }

private:

    struct DeviceRank
    {
        uint16_t deviceScore = 0;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    };
    static bool rankComparator(const DeviceRank &r1, const DeviceRank &r2) { return r1.deviceScore > r2.deviceScore; }

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    int m_graphicsQueueFamilyIndex = -1;
    int m_presentationQueueFamilyIndex = -1;

    bool hasRequiredFeatures(VkPhysicalDevice &physicalDevice,
        VkPhysicalDeviceProperties &deviceProperties,
        VkPhysicalDeviceFeatures &deviceFeatures,
        uint16_t &deviceScore);

    bool findQueueFamilies(VkQueueFlags requiredQueueFamilyFlags, VkSurfaceKHR surface);
};

#endif // VULKANPHYSICALDEVICE_H