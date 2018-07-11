#include "VulkanCommandPool.h"

#include <assert.h>
#include <iostream>

bool VulkanCommandPool::init(VkDevice device, int queueFamilyIndex)
{
    // Commands created from this command pool can be submitted only to 
    // a single type of queue (graphics, compute, etc)
    assert(queueFamilyIndex != -1 && "Invalid graphics queue family index");

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    commandPoolCreateInfo.flags = 0;

    // Create command pool
    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        std::cout << "Failed to create command pool.\n";
        return false;
    }

    // Success
    return true;
}
    
void VulkanCommandPool::cleanup(VkDevice device)
{
    if (m_commandPool != VK_NULL_HANDLE)
        vkDestroyCommandPool(device, m_commandPool, nullptr);
}