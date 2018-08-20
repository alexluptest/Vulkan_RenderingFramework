#include "Quad.h"

#include <iostream>

bool Quad::init(VkDevice device,
    uint32_t width, uint32_t height, 
    VkRenderPass renderPass,
    const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo)
{
    // Create pipeline layout
    if (createPipelineLayout(device) == false) return false;

    // Create graphics pipeline
    if (createGraphicsPipeline(device, width, height, renderPass, shaderStagesInfo) == false) return false;

    // Success
    return true;
}
    
void Quad::cleanup(VkDevice device)
{
    // Descriptor set layout
    if (m_descriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);

    // Pipeline layout
    if (m_pipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
}

void Quad::render()
{

}

bool Quad::createPipelineLayout(VkDevice device)
{
    // Descriptor binding  
    //  - binding - number that corresponds to a resource of the same type in the shader stages
    //  E.g. GLSL: layout (set=1, binding=1) uniform uniformBuffer

	//	- descriptor type - specifies the type of resource descriptors used for this binding
    //  E.g. VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, etc.

    //	- count - number of descriptors contained in the binding that will be accessed in the shader as an array.
    //  E.g. GLSL: layout (set=0, binding=1) uniform texture2D sampledImagesArray[12];

	//	- set of shaders that can access the binding
    //  E.g. VK_SHADER_STAGE_FRAGMENT_BIT, VK_SHADER_STAGE_VERTEX_BIT, etc.

    //  - set of immutable samplers
    //  Used when we don't want to update the samplers dynamically

    // Setup an array of descriptor bindings
    const VkDescriptorSetLayoutBinding layoutBindings[] = 
    {
        // Descriptor binding to a uniform buffer
        {
            0,                                      // binding
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,      // descriptorType
            1,                                      // descriptorCount
            VK_SHADER_STAGE_VERTEX_BIT,             // stageFlags
            nullptr                                 // pImmutableSamplers
        }
    };

    // Descriptor set layout - defines the types and number of descriptors in a descriptor set
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    // In the current example we'll only have one decriptor set, so only one element of type VkDescriptorSetLayoutCreateInfo
    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo[] = 
    {
        // Descriptor set layout create into for our descriptor set that contains one binding
        {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,        // sType
            nullptr,                                                    // pNext
            0,                                                          // flags
            1,                                                          // bindingCount
            &layoutBindings[0]                                          // pBindings
        }
    };
    // Create the descriptor set layout
    if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo[0], nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
    {
        std::cout << "Failed to create the quad descriptor set layout. \n";
        return false;
    }

    // Pipeline layout
    //  - describes the set of resources that can be accessed by a pipeline
	//  - gives access to a descriptor set from a pipeline
	//  - the sequence of descriptor set layouts determines the interface between the shader stages and shader resources
	//  - each pipeline is created based on a pipeline layout
    
    // Structure
    // pipeline layout
	// {
    //      descriptor_set_layout0 descriptor_set_layout1 ... descriptor_set_layoutN
	//      push_constant_range0 push_constant_range1 ... push_constant_rangeN
    // }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,			// sType
	    nullptr,												// pNext
	    0,														// flags
	    1,														// setLayoutCount													 // sets included in the pipeline
	    &m_descriptorSetLayout,									// pSetLayouts 														// descriptor set layouts
	    0,														// pushConstantRangeCount
	    nullptr													// pPushConstantRanges
    };
    // Create the pipeline layout
    if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        std::cout << "Failed to create pipeline layout.\n";
        return false;
    }

    // Success
    return true;
}

bool Quad::createGraphicsPipeline(VkDevice device, 
    uint32_t width, uint32_t height, 
    VkRenderPass renderPass,
    const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo)
{
    // Depth stencil state
    DepthStencilState depthStencilState = {};

    // Vertex input state
    VertexInputState vertexInputState = {};
    vertexInputState.vertexBindingDescriptions = VertexPC::getBindingDescription();
    vertexInputState.vertexAttributeDescriptions = VertexPC::getAttributeDescriptions();

    // Define blend states

    // No blending
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        VK_FALSE,                           // blendEnable
        VK_BLEND_FACTOR_ONE,                // srcColorBlendFactor
        VK_BLEND_FACTOR_ZERO,               // dstColorBlendFactor
        VK_BLEND_OP_ADD,                    // colorBlendOp
        VK_BLEND_FACTOR_ONE,                // srcAlphaBlendFactor
        VK_BLEND_FACTOR_ZERO,               // dstAlphaBlendFactor
        VK_BLEND_OP_ADD,                    // alphaBlendOp
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT   // colorWriteMask
    };

    // Alpha blending
    // finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor
    // finalColor.a = newAlpha
    VkPipelineColorBlendAttachmentState colorBlendAlphaBlendingAttachmentState = {
        VK_FALSE,                               // blendEnable
        VK_BLEND_FACTOR_SRC_ALPHA,              // srcColorBlendFactor
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,    // dstColorBlendFactor
        VK_BLEND_OP_ADD,                        // colorBlendOp
        VK_BLEND_FACTOR_ONE,                    // srcAlphaBlendFactor
        VK_BLEND_FACTOR_ZERO,                   // dstAlphaBlendFactor
        VK_BLEND_OP_ADD,                        // alphaBlendOp
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT   // colorWriteMask
    };

    // Blending states list - one per attachment
    std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates = {
        colorBlendAttachmentState
    };

    // Dynamic states
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT
    };

    // Initialize graphics pipeline
    if (m_pipeline.init(device, 
        width, height, 
        vertexInputState, 
        depthStencilState, 
        shaderStagesInfo,
        blendAttachmentStates,
        dynamicStates,
        VK_SAMPLE_COUNT_1_BIT,
        m_pipelineLayout,
        renderPass) == false) return false;

    // Success
    return true;
}