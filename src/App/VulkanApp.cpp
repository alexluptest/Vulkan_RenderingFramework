#include "VulkanApp.h"

#include "VertexFormat.h"

VulkanApp::VulkanApp()
{
    
}

VulkanApp::~VulkanApp()
{

}

bool VulkanApp::init(VkDevice device)
{
    // Shaders
    if (m_quadVertexShader.init(device, "./shaders/binaries/triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT) == 0) return false;
    if (m_quadFragmentShader.init(device, "./shaders/binaries/triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT) == 0) return false;

    VkPipelineShaderStageCreateInfo shaderStagesInfo[] = { 
        m_quadVertexShader.shaderStageInfo(),
        m_quadFragmentShader.shaderStageInfo() 
    };




    // ---------------------------------
    // TEMP - remove
    // Create descriptor pool
    /*size_t imageCount = 2;
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.resize(1);
    poolSizes[0].descriptorCount = imageCount;
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if (m_descriptorPool.init(device, poolSizes, imageCount) == false)
        return false;
    // Create descriptor set
    std::vector<VkDescriptorSetLayout> descSetLayouts = { m_quadDescriptorSetLayout, m_quadDescriptorSetLayout };
    if (m_descriptorSets.init(device, m_descriptorPool, descSetLayouts) == false)
        return false;*/
    // ---------------------------------

}

void VulkanApp::cleanup()
{

}