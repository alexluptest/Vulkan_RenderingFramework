#include "Quad.h"

#include <iostream>

#include "VulkanLogicalDevice.h"

Quad::Quad(VkDevice device)
    : m_logicalDevice(device)
{

}

bool Quad::init(const VulkanEngine &engine,
    uint32_t width, uint32_t height, 
    const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo)
{
    m_width = width;
    m_height = height;

    // Create pipeline layout
    if (createPipelineLayout(engine.device()) == false) return false;

    // Create graphics pipeline
    if (createGraphicsPipeline(engine.device(), width, height, engine.renderPass().get(), shaderStagesInfo) == false) return false;

    // Create descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
    };
    if (m_descriptorPool.init(engine.device(), poolSizes, poolSizes.size()) == false)
        return false;

    // Create descriptor set
    if (m_descriptorSets.init(engine.device(), m_descriptorPool, { m_descriptorSetLayout }) == false)
        return false;

    // Setup geometry
    if (setupGeometry(engine.physicalDevice(), engine.device(), engine.graphicsQueue(), engine.framesInFlight()) == false)
        return false;

    // Success
    return true;
}
    
void Quad::cleanup()
{
    // Descriptor set layout
    if (m_descriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(m_logicalDevice, m_descriptorSetLayout, nullptr);

    // Pipeline layout
    if (m_pipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);

    // Descritpor pool
    m_descriptorPool.cleanup(m_logicalDevice);
}

void Quad::render(VkCommandBuffer currentCommandBuffer) const
{
    // Set dynamic viewport
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = m_width;
    viewport.height = m_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);
    // Bind the pipline
    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.get());
    // Bind the quad vertex buffer
    VkBuffer vertexBuffers[] = { m_quadVertexBuffer.get() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, vertexBuffers, offsets);
    // Bind the quad index buffer
    vkCmdBindIndexBuffer(currentCommandBuffer, m_quadIndexBuffer.get(), 0, VK_INDEX_TYPE_UINT16);
    // Send the draw command
    vkCmdDrawIndexed(currentCommandBuffer, m_quadIndexBuffer.elementCount(), 1, 0, 0, 0);
}

void Quad::update(double dt, uint32_t frameIndex)
{
    // Update uniform data
    if (m_quadUniformBuffer.updateUniformData(m_logicalDevice, 
            frameIndex,
            reinterpret_cast<void*>(&m_quadUniformData),
            sizeof(UniformBufferObject)) == 0)
        std::cout << "Failed to update uniform data.\n";
}

bool Quad::setupGeometry(const VulkanPhysicalDevice &physicalDevice, VkDevice device, const VulkanQueue &graphicsQueue, uint32_t framesInFlight)
{
    // Triangle vertex buffer setup
    std::vector<VertexPC> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    // Init vertex buffer
    if (m_quadVertexBuffer.init(physicalDevice, 
            device, 
            sizeof(vertices[0]),
            vertices.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            reinterpret_cast<void*>(vertices.data()),
            graphicsQueue) == 0) return false;
    // Init index buffer
    if (m_quadIndexBuffer.init(physicalDevice,
            device,
            sizeof(indices[0]),
            indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            reinterpret_cast<void*>(indices.data()),
            graphicsQueue) == 0) return false;
    // Init uniform buffer
    if (m_quadUniformBuffer.init(physicalDevice,
            device,
            sizeof(UniformBufferObject),
            framesInFlight,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            reinterpret_cast<void*>(&m_quadUniformData),
            graphicsQueue) == 0) return false;

    // Image
    if (m_testImage.init(physicalDevice, 
        device, 
        VK_IMAGE_TYPE_2D, 
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        300, 300, 1, 1, 1, VK_SAMPLE_COUNT_1_BIT, 
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == false) return false;

    m_testImage.createView(device, VK_IMAGE_ASPECT_COLOR_BIT);

    // Success
    return true;
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