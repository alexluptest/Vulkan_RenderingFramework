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

    inline const VkRenderPass get() const { return m_renderPass; }

private:

    VkRenderPass m_renderPass = VK_NULL_HANDLE;

};

#endif // VULKANRENDERPASS_H