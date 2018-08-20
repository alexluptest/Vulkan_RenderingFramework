#include "VulkanGraphicsPipeline.h"

#include <iostream>

bool VulkanGraphicsPipeline::init(VkDevice device, 
    uint32_t width, uint32_t height,
    const VertexInputState &vertexInputState,
    const DepthStencilState &depthStencilState,
    const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo,
    const std::vector<VkPipelineColorBlendAttachmentState> &colorBlendStates,
    const std::vector<VkDynamicState> &dynamicStates,
    VkSampleCountFlagBits sampleCount,
    VkPipelineLayout pipelineLayout,
    VkRenderPass &renderPass)
{
    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,                      // sType
        nullptr,                                                                        // pNext
        0,                                                                              // flags
        // Bindings - spacing between data and whether the data is per vertex or per instance
        static_cast<uint32_t>(vertexInputState.vertexBindingDescriptions.size()),       // vertexBindingDescriptionCount
        vertexInputState.vertexBindingDescriptions.data(),                              // pVertexBindingDescriptions
        // Attrib description - types of attributes passed to the vertex shader, which binding to load them from and at which offset
        static_cast<uint32_t>(vertexInputState.vertexAttributeDescriptions.size()),     // vertexAttributeDescriptionCount
        vertexInputState.vertexAttributeDescriptions.data()                             // pVertexAttributeDescriptions
    };

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,                    // sType
        nullptr,                                                                        // pNext
        0,                                                                              // flags
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                                            // topology
        VK_FALSE                                                                        // primitiveRestartEnable
    };

    // Viewport and scissor

    // Configure default fullscreen viewport and scissor size to use when they are not set dynamically
    VkViewport viewport = {
        0.0f,       // x
        0.0f,       // y
        width,      // width
        height,     // height
        0.0f,       // minDepth
        1.0f        // maxDepth
    };
    VkRect2D scissor = {
        { 0, 0 },           // offset
        { width, height }   // extent
    };
    VkPipelineViewportStateCreateInfo viewportStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,      // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        1,                                                          // viewportCount
        &viewport,                                                  // pViewports
        1,                                                          // scissorCount
        &scissor                                                    // pScissors
    };

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizerStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        VK_FALSE,                                                   // depthClampEnable
        VK_FALSE,                                                   // rasterizerDiscardEnable
        VK_POLYGON_MODE_FILL,                                       // polygonMode
        VK_CULL_MODE_BACK_BIT,                                      // cullMode
        VK_FRONT_FACE_CLOCKWISE,                                    // frontFace
        VK_FALSE,                                                   // depthBiasEnable
        0.0f,                                                       // depthBiasConstantFactor
        0.0f,                                                       // depthBiasClamp
        0.0f,                                                       // depthBiasSlopeFactor
        1.0f                                                        // lineWidth
    };

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,   // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        sampleCount,                                                // rasterizationSamples
        VK_FALSE,                                                   // sampleShadingEnable
        1.0f,                                                       // minSampleShading
        nullptr,                                                    // pSampleMask
        VK_FALSE,                                                   // alphaToCoverageEnable
        VK_FALSE                                                    // alphaToOneEnable
    };

    // Depth and stencil
    VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        depthStencilState.depthTestEnabled,                         // depthTestEnable
        depthStencilState.depthWriteEnabled,                        // depthWriteEnable
        depthStencilState.depthCompareOp,                           // depthCompareOp
        VK_FALSE,                                                   // depthBoundsTestEnable
        depthStencilState.stencilTestEnabled,                       // stencilTestEnable
        depthStencilState.stencilFrontOpState,                      // front
        depthStencilState.stencilBackOpState,                       // back
        0.0f,                                                       // minDepthBounds
        1.0f                                                        // maxDepthBounds
    };

    // Color blending state
    VkPipelineColorBlendStateCreateInfo colorBlendingStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        VK_FALSE,                                                   // logicOpEnable
        VK_LOGIC_OP_COPY,                                           // logicOp
        static_cast<uint32_t>(colorBlendStates.size()),             // attachmentCount
        colorBlendStates.data(),                                    // pAttachments
        { 0.0f, 0.0f, 0.0f, 0.0f }                                  // blendConstants[4]
    };

    // Configuration for these values will be ignored and these will be specified at draw time
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,       // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        static_cast<uint32_t>(dynamicStates.size()),                // dynamicStateCount
        dynamicStates.data()                                        // pDynamicStates
    };

    // Graphics pipeline info
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,            // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        static_cast<uint32_t>(shaderStagesInfo.size()),             // stageCount
        shaderStagesInfo.data(),                                    // pStages
        &vertexInputStateInfo,                                      // pVertexInputState
        &inputAssemblyStateInfo,                                    // pInputAssemblyState
        nullptr,                                                    // pTesselationState
        &viewportStateInfo,                                         // pViewportState
        &rasterizerStateInfo,                                       // pRasterizationState
        &multisampleStateInfo,                                      // pMultisamplState
        &depthStencilStateInfo,                                     // pDepthStencilState
        &colorBlendingStateInfo,                                    // pColorBlendState
        &dynamicStateInfo,                                          // pDynamicState
        pipelineLayout,                                             // layout
        renderPass,                                                 // renderPass
        0,                                                          // subpass
        VK_NULL_HANDLE,                                             // basePipelineHandle
        -1                                                          // basePipelineIndex
    };
    // Create graphics pipeline
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
    {
        std::cout << "Failed to create the graphics pipeline. \n";
        return false;
    }

    // Success
    return true;
}
    
void VulkanGraphicsPipeline::cleanup(VkDevice device)
{
    if (m_graphicsPipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(device, m_graphicsPipeline, nullptr);
}