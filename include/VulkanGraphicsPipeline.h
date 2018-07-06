#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#include "VulkanHelper.h"
#include "VulkanShader.h"

class VulkanGraphicsPipeline
{

public:

    VulkanGraphicsPipeline() {}
    ~VulkanGraphicsPipeline() {}

    bool init(const VulkanShader &vertexShader, const VulkanShader &fragmentShader);
    void cleanup();

private:

};

#endif // VULKANGRAPHICSPIPELINE_H