#ifndef VULKANDESCRIPTORSETS_H
#define VULKANDESCRIPTORSETS_H

#include "VulkanHelper.h"
#include "VulkanDescriptorPool.h"

#include <vector>

class VulkanDescriptorSets
{

public:

    VulkanDescriptorSets() = default;
    ~VulkanDescriptorSets() = default;

    bool init(VkDevice device, const VulkanDescriptorPool &descriptorPool, std::vector<VkDescriptorSetLayout> descriptorLayouts);
    void setBuffer(VkDevice device, uint32_t descriptorSetIndex, uint32_t binding, uint32_t elementIndex, uint32_t descriptorCount);
    void updateDescriptorSets(VkDevice device);

private:

    std::vector<VkDescriptorSet> m_descriptorSets;

    std::vector<VkWriteDescriptorSet> m_writeSets;
    std::vector<VkCopyDescriptorSet> m_copySets;

};

#endif // VULKANDESCRIPTORSET_H