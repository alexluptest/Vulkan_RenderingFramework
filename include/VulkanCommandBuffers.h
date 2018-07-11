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

private:

    std::vector<VkCommandBuffer> m_commandBuffers;

};

#endif // VULKANCOMMANDBUFFERS_H