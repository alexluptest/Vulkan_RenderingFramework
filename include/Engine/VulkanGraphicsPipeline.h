#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#include "VulkanHelper.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VertexFormat.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSets.h"

// ----------------------------------------------------------------------------
// Helper structures used to initialize the graphics pipeline

// Structure that contains information used to initialize the VkPipelineVertexInputStateCreateInfo
struct VertexInputState
{
    // Vertex bindings desc
    std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
    // Vertex attributes desc
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
};

// Structure that contains information about how the depth stencil buffers should operate
struct DepthStencilState
{
    bool depthTestEnabled = true;
    bool depthWriteEnabled = true;
    VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
    bool stencilTestEnabled = false;
    VkStencilOpState stencilFrontOpState = {};
    VkStencilOpState stencilBackOpState = {};
};

// ----------------------------------------------------------------------------

class VulkanGraphicsPipeline
{

public:

    VulkanGraphicsPipeline() {}
    ~VulkanGraphicsPipeline() {}

    inline const VkPipeline get() const { return m_graphicsPipeline; }

    bool init(VkDevice device, 
        uint32_t width, uint32_t height,
        const VertexInputState &vertexInputState,
        const DepthStencilState &depthStencilState,
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo,
        const std::vector<VkPipelineColorBlendAttachmentState> &colorBlendStates,
        const std::vector<VkDynamicState> &dynamicStates,
        VkSampleCountFlagBits sampleCount,
        VkPipelineLayout pipelineLayout,
        VkRenderPass &renderPass);
    void cleanup(VkDevice device);

private:

    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
};

#endif // VULKANGRAPHICSPIPELINE_H