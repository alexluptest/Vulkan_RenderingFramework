#include "VulkanCommandBuffers.h"

#include <iostream>
#include <assert.h>

bool VulkanCommandBuffers::init(VkDevice device, VkCommandPool storageCommandPool, uint32_t bufferCount)
{
    assert(bufferCount > 0 && "Invalid buffer count.");

    m_commandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = bufferCount;
    commandBufferAllocateInfo.commandPool = storageCommandPool;

    // Create command buffers
    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        std::cout << "Failed to allocate command buffers.\n";
        return false;
    }

    // Success
    return true;
}