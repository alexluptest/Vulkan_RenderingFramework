#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#include "VulkanHelper.h"
#include "VulkanShader.h"

class VulkanGraphicsPipeline
{

public:

    VulkanGraphicsPipeline() {}
    ~VulkanGraphicsPipeline() {}

    bool init(VkDevice device, uint32_t width, uint32_t height, const VulkanShader &vertexShader, const VulkanShader &fragmentShader);
    void cleanup(VkDevice device);

private:

    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

};

#endif // VULKANGRAPHICSPIPELINE_H