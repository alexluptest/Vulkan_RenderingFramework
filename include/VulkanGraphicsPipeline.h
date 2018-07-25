#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#include "VulkanHelper.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VertexFormat.h"
#include "VulkanBuffer.h"

class VulkanGraphicsPipeline
{

public:

    VulkanGraphicsPipeline() {}
    ~VulkanGraphicsPipeline() {}

    inline const VkPipeline get() const { return m_graphicsPipeline; }

    bool init(VkDevice device, 
        uint32_t width, uint32_t height, 
        const VulkanShader &vertexShader, const VulkanShader &fragmentShader,
        const VulkanRenderPass &renderPass);
    void cleanup(VkDevice device);

private:

    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
};

#endif // VULKANGRAPHICSPIPELINE_H