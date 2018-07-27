#ifndef VULKANCOMMANDBUFFERS_H
#define VULKANCOMMANDBUFFERS_H

#include "VulkanHelper.h"
#include "VulkanCommandPool.h"

#include <vector>

class VulkanCommandBuffers
{

public:

    VulkanCommandBuffers() {}
    ~VulkanCommandBuffers() {}

    bool init(VkDevice device, VkCommandPool commandPool, uint32_t bufferCount);
    bool beginCommandBuffer(uint32_t commandBufferIndex, VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    bool endCommandBuffer(uint32_t commandBufferIndex);

    const inline std::vector<VkCommandBuffer> &get() const { return m_commandBuffers; }

private:

    std::vector<VkCommandBuffer> m_commandBuffers;

};

#endif // VULKANCOMMANDBUFFERS_H