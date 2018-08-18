#include "VulkanDescriptorSets.h"
#include <assert.h>
#include <iostream>

bool VulkanDescriptorSets::init(VkDevice device, const VulkanDescriptorPool &descriptorPool, std::vector<VkDescriptorSetLayout> descriptorLayouts)
{
    assert(descriptorLayouts.size() > 0 && "Invalid descriptor set count.");

    // Descriptor set allocate info
    VkDescriptorSetAllocateInfo descSetAllocateInfo = {};
    descSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descSetAllocateInfo.pSetLayouts = descriptorLayouts.data();
    descSetAllocateInfo.descriptorSetCount = descriptorLayouts.size();
    descSetAllocateInfo.descriptorPool = descriptorPool.get();

    // Create descriptor sets
    m_descriptorSets.resize(descriptorLayouts.size());
    if (vkAllocateDescriptorSets(device, &descSetAllocateInfo, &m_descriptorSets[0]) != VK_SUCCESS)
    {
        std::cout << "Failed to allocate descriptor sets.\n";
        return false;
    }

    // Success
    return true;
}