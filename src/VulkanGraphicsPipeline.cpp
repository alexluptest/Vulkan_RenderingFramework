#include "VulkanGraphicsPipeline.h"

#include <iostream>

bool VulkanGraphicsPipeline::init(VkDevice device, 
    uint32_t width, uint32_t height, 
    const VulkanShader &vertexShader, const VulkanShader &fragmentShader,
    const VulkanRenderPass &renderPass)
{
    // Vertex input
    auto vertexAttributes = VertexPC::getAttributeDescriptions();
    auto vertexBindings = VertexPC::getBindingDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // Bindings - spacing between data and whether the data is per vertex or per instance
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBindings;
    // Attrib description - types of attributes passed to the vertex shader,which
    // binding to load them from and at which offset
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributes.data();

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    // Viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent.width = width;
    scissor.extent.height = height;
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizerStateCreateInfo = {};
    rasterizerStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizerStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerStateCreateInfo.lineWidth = 1.0f;
    rasterizerStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizerStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizerStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizerStateCreateInfo.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.minSampleShading = 1.0f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    // Depth and stencil
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};

    // Color blending

    // The blending structures - one per attached framebuffer and a global one

    // No blending
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    // The new color from the fragment shader is passed to the framebuffer
    // unmodified
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    // Color blending
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    // Alpha blending
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    // Alpha blending

    // finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor
    // finalColor.a = newAlpha
    VkPipelineColorBlendAttachmentState colorBlendAlphaBlendingAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    // The new color from the fragment shader is passed to the framebuffer
    // unmodified
    colorBlendAlphaBlendingAttachmentState.blendEnable = VK_FALSE;
    // Color blending
    colorBlendAlphaBlendingAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAlphaBlendingAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAlphaBlendingAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    // Alpha blending
    colorBlendAlphaBlendingAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAlphaBlendingAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAlphaBlendingAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    // Global blend state
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachmentState;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Dynamic state
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    // Configuration for these values will be ignored and these will be specified at draw time
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStates;
    dynamicState.dynamicStateCount = 2;

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        std::cout << "Failed to create a pipeline layout. \n";
        return false;
    }

    // Graphics pipeline
    VkPipelineShaderStageCreateInfo stages[] = { 
        vertexShader.shaderStageInfo(),
        fragmentShader.shaderStageInfo() 
    };

    // ---------------------------------
    // TEMP - remove
    // Descriptor layout, descriptor set layout, pipeline layout
    if (createDescriptorSetLayout(device) == 0)
        return false;
    // Create descriptor pool
    size_t imageCount = 2;
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.resize(1);
    poolSizes[0].descriptorCount = imageCount;
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if (m_descriptorPool.init(device, poolSizes, imageCount) == false)
        return false;
    // Create descriptor set
    std::vector<VkDescriptorSetLayout> descSetLayouts = { m_quadDescriptorSetLayout, m_quadDescriptorSetLayout };
    if (m_descriptorSets.init(device, m_descriptorPool, descSetLayouts) == false)
        return false;
    // ---------------------------------

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = stages;
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizerStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlending;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = m_quadPipelineLayout;
    pipelineCreateInfo.renderPass = renderPass.get();
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    // Create graphics pipeline
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
    {
        std::cout << "Failed to create the graphics pipeline. \n";
        return false;
    }

    // Success
    return true;
}

bool VulkanGraphicsPipeline::createDescriptorSetLayout(VkDevice device)
{
    // Every binding needs to be described using a VkDescriptorSetLayoutBinding
    VkDescriptorSetLayoutBinding descriptorBinding = {};
    descriptorBinding.binding = 0;
    descriptorBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorBinding.descriptorCount = 1;
    descriptorBinding.pImmutableSamplers = nullptr;
    descriptorBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // Descriptor set layout
    // All descriptor bindings are combined into a descriptor set layout
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.flags = 0;
    descriptorSetLayoutCreateInfo.pBindings = &descriptorBinding;

    // Create a descriptor set layout
    if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &m_quadDescriptorSetLayout) != VK_SUCCESS)
    {
        std::cout << "Failed to create the descriptor set layout. \n";
        return false;
    }

    // Create the pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_quadDescriptorSetLayout;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_quadPipelineLayout) != VK_SUCCESS)
    {
        std::cout << "Failed to create pipeline layout. \n";
        return false;
    }

    // Success
    return true;
}
    
void VulkanGraphicsPipeline::cleanup(VkDevice device)
{
    if (m_graphicsPipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(device, m_graphicsPipeline, nullptr);
    if (m_pipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);

    // Destroy the descriptor set layout
    if (m_quadDescriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(device, m_quadDescriptorSetLayout, nullptr);
    if (m_quadPipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(device, m_quadPipelineLayout, nullptr);
}