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

private:

    std::vector<VkDescriptorSet> m_descriptorSets;

};

#endif // VULKANDESCRIPTORSET_H