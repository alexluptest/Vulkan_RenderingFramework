#ifndef QUAD_H
#define QUAD_H

#include "VulkanHelper.h"
#include "VulkanGraphicsPipeline.h"

class Quad
{

public:

    Quad() = default;
    ~Quad() = default;

    bool init(VkDevice device,
        uint32_t width, uint32_t height, 
        VkRenderPass renderPass,
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo);
    void cleanup(VkDevice device);
    void render();

private:

    // Members
    VulkanGraphicsPipeline m_pipeline;
    
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

    // Methods
    bool createPipelineLayout(VkDevice device);
    bool createGraphicsPipeline(VkDevice device, 
        uint32_t width, uint32_t height, 
        VkRenderPass renderPass,
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo);

};

#endif // QUAD_H