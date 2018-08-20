#ifndef VULKANDESCRIPTORPOOL_H
#define VULKANDESCRIPTORPOOL_H

#include "VulkanHelper.h"
#include <vector>

class VulkanDescriptorPool
{
public:

    VulkanDescriptorPool() = default;
    ~VulkanDescriptorPool() = default;

    bool init(VkDevice device, std::vector<VkDescriptorPoolSize>, size_t descriptorSetCount);
    void cleanup(VkDevice device);

    const auto inline get() const { return m_descriptorPool; }

private:

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;

};

#endif // VULKANDESCRIPTORPOOL_H