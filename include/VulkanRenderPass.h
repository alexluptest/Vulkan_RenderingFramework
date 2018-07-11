#ifndef VULKANRENDERPASS_H
#define VULKANRENDERPASS_H

#include "VulkanHelper.h"

class VulkanRenderPass
{

public:

    VulkanRenderPass() {}
    ~VulkanRenderPass() {}

    bool init(VkDevice device, VkFormat swapChainFormat);
    void cleanup(VkDevice device);

private:

    VkRenderPass m_renderPass = VK_NULL_HANDLE;

};

#endif // VULKANRENDERPASS_H