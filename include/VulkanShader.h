#ifndef VULKANSHADER_H
#define VULKANSHADER_H

#include "VulkanHelper.h"

#include <fstream>
#include <vector>
#include <string>

class VulkanShader
{

public:

    VulkanShader() {}
    ~VulkanShader() {}

    bool init(VkDevice device, const std::string &shaderFilename, VkShaderStageFlagBits shaderStage);
    void cleanup(VkDevice device);

    inline const VkShaderModule shaderModule() const { return m_shaderModule; }
    inline const VkPipelineShaderStageCreateInfo shaderStageInfo() const { return m_shaderStageCreateInfo; }

private:

    std::vector<char> readFile(const std::string &filename);

    VkShaderModule m_shaderModule = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo m_shaderStageCreateInfo;

    bool createShaderModule(VkDevice device,
        const std::string &shaderFilename);
};

#endif // VULKANSHADER_H