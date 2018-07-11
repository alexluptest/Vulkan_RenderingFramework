#ifndef VULKANCOMMANDPOOL_H
#define VULKANCOMMANDPOOL_H

#include "VulkanHelper.h"

class VulkanCommandPool
{

public:

    VulkanCommandPool() {}
    ~VulkanCommandPool() {}

    const inline VkCommandPool get() const { return m_commandPool; }

    bool init(VkDevice device, int graphicsQueueFamilyIndex);
    void cleanup(VkDevice device);

private:

    VkCommandPool m_commandPool = VK_NULL_HANDLE;

};

#endif // VULKANCOMMANDPOOL_H