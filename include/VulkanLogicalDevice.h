#ifndef VULKANLOGICALDEVICE_H
#define VULKANLOGICALDEVICE_H

#include "VulkanHelper.h"

#include <vector>

class VulkanLogicalDevice
{
public:

    VulkanLogicalDevice() {}
    ~VulkanLogicalDevice() {}

    bool init(VkPhysicalDevice physicalDevice,
        uint32_t queueFamilyIndex, 
        uint32_t queueCount);
    void cleanup();

    inline const VkDevice &get() const { return m_logicalDevice; }

private:

    VkDevice m_logicalDevice = VK_NULL_HANDLE;
    std::vector<VkExtensionProperties> m_supportedDeviceExtensions;

    bool checkDeviceExtensionSupport(const std::vector<const char*> &requiredDeviceExtensions);
    
};

#endif // VULKANLOGICALDEVICE_H