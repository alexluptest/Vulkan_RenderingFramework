#ifndef VULKANQUEUE_H
#define VULKANQUEUE_H

#include "VulkanHelper.h"
#include "VulkanCommandBuffers.h"

#include <vector>

struct VulkanSynchronizationObject
{
    std::vector<VkSemaphore> waitForObjects;
    std::vector<VkSemaphore> signalObjects;
    std::vector<VkFence> fences;

    bool init(VkDevice device, uint32_t waitForObjectCount, uint32_t signalObjectCount, uint32_t fencesCount);
    void cleanup(VkDevice device);
};

class VulkanQueue
{

public:

    VulkanQueue() {}
    ~VulkanQueue() {}

    void init(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex);
    bool submitCommandBuffers(const VulkanSynchronizationObject &syncObject, uint32_t currentFrameIndex, const std::vector<VkCommandBuffer> &commandBuffers);

    const inline VkQueue &graphicsQueueHandle() const { return m_queueHandle; }

private:

    VkQueue m_queueHandle;

};

#endif // VULKANQUEUE_H