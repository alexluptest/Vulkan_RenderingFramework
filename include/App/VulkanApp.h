#ifndef VULKANAPP_H
#define VULKANAPP_H

#include "VulkanHelper.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSets.h"

class VulkanApp
{

public:

    VulkanApp();
    ~VulkanApp();

    bool init(VkDevice device);
    void cleanup();

private:

    VulkanShader m_quadVertexShader, m_quadFragmentShader;

    VulkanDescriptorPool m_descriptorPool;
    VulkanDescriptorSets m_descriptorSets;

};

#endif // VULKANAPP_H