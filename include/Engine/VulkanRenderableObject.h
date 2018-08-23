#ifndef VULKANRENDERABLEOBJECT_H
#define VULKANRENDERABLEOBJECT_H

#include "VulkanHelper.h"
#include <vector>

class VulkanEngine;

class VulkanRenderableObject
{

public:

    VulkanRenderableObject() = default;
    virtual ~VulkanRenderableObject() = default;

    virtual bool init(const VulkanEngine &engine,
        uint32_t width, uint32_t height,
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo) = 0;
    virtual void cleanup() = 0;
    virtual void render(VkCommandBuffer currentCommandBuffer) const = 0;
    virtual void update(double dt, uint32_t frameIndex) = 0;

private:



};

#endif // VULKANRENDERABLEOBJECT_H