#include "VulkanRenderPass.h"

#include <iostream>

bool VulkanRenderPass::init(VkDevice device, VkFormat swapChainFormat)
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // What to do with the data in the attachment berfore rendering
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // What to do with the data in the attachment after rendering
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // ?
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE; // ?
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Attachment reference
    VkAttachmentReference colorAttachmentReference = {};
    // Index of the attachment in the attachment description array
    colorAttachmentReference.attachment = 0;
    // The layout of the attachment during the subpass
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Subpass description
    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

    // Dependency
    VkSubpassDependency dependency = {};
    // Implicit subpass before of after the render pass
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // Dependent subpass index
    dependency.dstSubpass = 0;
    // Specifies the stages to wait for
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // Specifies the operations to wait for
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Render pass
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;
    
    if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS)
    {
        std::cout << "Failed to create render pass.\n";
        return false;
    }

    // Success
    return true;
}
    
void VulkanRenderPass::cleanup(VkDevice device)
{
    if (m_renderPass != VK_NULL_HANDLE)
        vkDestroyRenderPass(device, m_renderPass, nullptr);
}