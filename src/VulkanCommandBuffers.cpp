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

bool VulkanCommandBuffers::beginCommandBuffer(uint32_t commandBufferIndex, VkCommandBufferUsageFlags flags)
{
    assert(commandBufferIndex < m_commandBuffers.size() && "Invalid command buffer index to start.\n");

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    // The command buffer can be submitted again while its execution is pending in another command buffer
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(m_commandBuffers[commandBufferIndex], &commandBufferBeginInfo) != VK_SUCCESS)
    {
        std::cout << "Failed to begin command buffer recording for command buffer " << commandBufferIndex << ".\n";
        return false;
    }

    // Success
    return true;
}

bool VulkanCommandBuffers::endCommandBuffer(uint32_t commandBufferIndex)
{
    assert(commandBufferIndex < m_commandBuffers.size() && "Invalid command buffer index to end.\n");

    if (vkEndCommandBuffer(m_commandBuffers[commandBufferIndex]) != VK_SUCCESS)
    {
        std::cout << "Failed to end command buffer recording for command buffer " << commandBufferIndex << ".\n";
        return false;
    }

    // Success
    return true;
}