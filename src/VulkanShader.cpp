#include "VulkanShader.h"

#include <assert.h>
#include <iostream>

bool VulkanShader::init(VkDevice device, 
    const std::string &shaderFilename,
    VkShaderStageFlagBits shaderStage)
{
    // Shader module
    if (createShaderModule(device, shaderFilename) == 0)
        return false;

    // Shader stage
    m_shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_shaderStageCreateInfo.stage = shaderStage;
    m_shaderStageCreateInfo.pName = "main";
    m_shaderStageCreateInfo.module = m_shaderModule;
    m_shaderStageCreateInfo.pSpecializationInfo = nullptr;

    // Success
    return true;
}

void VulkanShader::cleanup(VkDevice device)
{
    vkDestroyShaderModule(device, m_shaderModule, nullptr);
}

bool VulkanShader::createShaderModule(VkDevice device,
    const std::string &shaderFilename)
{
    // Read the spir-v binary
    auto vertexShaderBinary = readFile(shaderFilename);

    if (vertexShaderBinary.size() != 0)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t*>(vertexShaderBinary.data());
        shaderModuleCreateInfo.codeSize = vertexShaderBinary.size();

        if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
        {
            std::cout << "Failed to create shader module for: " << shaderFilename << ".\n";
            return false;
        }
    }
    else
    {
        std::cout << "Failed to read the shader binaries. \n";
        return false;
    }
    
    // Success
    return true;
}

std::vector<char> VulkanShader::readFile(const std::string &filename)
{
    assert(filename.length() != 0 && "Invalid file path.");

    std::vector<char> shaderBinary;

    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (file.is_open() == false)
    {
        std::cout << "Unable to open file: " << filename << "\n";
        return shaderBinary;
    }

    // Returns the position of the current character in the file
    size_t fileSize = (size_t)file.tellg();
    shaderBinary.resize(fileSize);
    // Go to the beginning of the file and read the whole file
    file.seekg(0);
    file.read(shaderBinary.data(), fileSize);
    // Close the file
    file.close();

    return shaderBinary;
}