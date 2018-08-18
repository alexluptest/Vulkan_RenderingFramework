#include "VulkanDescriptorPool.h"
#include <assert.h>
#include <iostream>

bool VulkanDescriptorPool::init(VkDevice device, 
    std::vector<VkDescriptorPoolSize> descriptorInfo,     // Structure that describes the max number of descriptors of each type that can be allocated from the pool
    size_t descriptorSetCountMax)                       // Max number of descriptor sets that can be allocated from this pool
{
    assert(descriptorInfo.size() != 0 && "Invalid descriptor pool count.");

    // Descriptor pool info
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = descriptorSetCountMax;
    descriptorPoolCreateInfo.poolSizeCount = descriptorInfo.size();
    descriptorPoolCreateInfo.pPoolSizes = descriptorInfo.data();

    // Create descriptor pool
    if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
    {
        std::cout << "Failed to create descriptor pool.\n";
        return false;
    }

    // Success
    return true;
}

void VulkanDescriptorPool::cleanup(VkDevice device)
{
    if (m_descriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
}