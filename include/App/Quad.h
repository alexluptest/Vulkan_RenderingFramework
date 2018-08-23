#ifndef QUAD_H
#define QUAD_H

#include "VulkanHelper.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSets.h"
#include "VulkanImage.h"
#include "VulkanEngine.h"
#include "VulkanRenderableObject.h"

class Quad : public VulkanRenderableObject
{

public:

    Quad(VkDevice device);
    ~Quad() override = default;

    bool init(const VulkanEngine &engine,
        uint32_t width, uint32_t height,
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo) override;
    void cleanup() override;
    void render(VkCommandBuffer currentCommandBuffer) const override;
    void update(double dt, uint32_t frameIndex) override;

private:

    // Uniform buffer object
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    // Members
    VulkanGraphicsPipeline m_pipeline;
    VulkanDescriptorPool m_descriptorPool;
    VulkanDescriptorSets m_descriptorSets;

    VulkanBuffer m_quadVertexBuffer;
    VulkanBuffer m_quadIndexBuffer;
    VulkanBuffer m_quadUniformBuffer;
    VulkanImage m_testImage;

    UniformBufferObject m_quadUniformData;
    
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

    VkDevice m_logicalDevice;

    uint32_t m_width, m_height;

    // Methods
    bool setupGeometry(const VulkanPhysicalDevice &physicalDevice, VkDevice device, const VulkanQueue &graphicsQueue, uint32_t framesInFlight);
    bool createPipelineLayout(VkDevice device);
    bool createGraphicsPipeline(VkDevice device, 
        uint32_t width, uint32_t height, 
        VkRenderPass renderPass,
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo);

};

#endif // QUAD_H