#ifndef VULKANQUEUE_H
#define VULKANQUEUE_H

#include "VulkanHelper.h"

class VulkanQueue
{

public:

    VulkanQueue() {}
    ~VulkanQueue() {}

    void init(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex);

    const inline VkQueue &graphicsQueueHandle() const { return m_graphicsQueueHandle; }

private:

    VkQueue m_graphicsQueueHandle;

};

#endif // VULKANQUEUE_H