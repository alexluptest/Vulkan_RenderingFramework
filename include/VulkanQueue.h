#ifndef VULKANQUEUE_H
#define VULKANQUEUE_H

#include "VulkanHelper.h"
#include "VulkanCommandBuffers.h"

#include <vector>

struct VulkanSynchronizationObject
{
    std::vector<VkSemaphore> waitForObjects;
    std::vector<VkSemaphore> signalObjects;

    bool init(VkDevice device, uint32_t waitForObjectCount, uint32_t signalObjectCount);
    void cleanup(VkDevice device);
};

class VulkanQueue
{

public:

    VulkanQueue() {}
    ~VulkanQueue() {}

    void init(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex);
    bool submitCommandBuffers(const VulkanSynchronizationObject &syncObject, const std::vector<VkCommandBuffer> &commandBuffers);

    const inline VkQueue &graphicsQueueHandle() const { return m_queueHandle; }

private:

    VkQueue m_queueHandle;

};

#endif // VULKANQUEUE_H