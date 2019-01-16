#include "VulkanDescriptorSets.h"
#include <assert.h>
#include <iostream>
#include <vector>

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

void VulkanDescriptorSets::setBuffer(VkDevice device, uint32_t descriptorSetIndex, uint32_t binding, uint32_t elementIndex, uint32_t descriptorCount)
{
    assert(descriptorSetIndex < m_descriptorSets.size());

    VkDescriptorBufferInfo bufferInfo[] = {
        //VkBuffer        buffer;
        //VkDeviceSize    offset;
        //VkDeviceSize    range;
    };

    std::vector<VkWriteDescriptorSet> writeSets = {
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
            nullptr,                                        // pNext
            m_descriptorSets[descriptorSetIndex],           // dstSet
            binding,                                        // dstBinding
            elementIndex,                                   // dstArrayElement
            descriptorCount,                                // descriptorCount
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,              // descriptorType
            nullptr,                                        // pImageInfo
            bufferInfo,                                     // pBufferInfo
            nullptr                                         // pTexelBufferView
        }
    };
}

void VulkanDescriptorSets::updateDescriptorSets(VkDevice device)
{
    vkUpdateDescriptorSets(device, m_writeSets.size(), m_writeSets.data(), m_copySets.size(), m_copySets.data());
}