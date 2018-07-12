#include "VulkanEngine.h"

#include <string>

bool VulkanEngine::initVulkan(const std::string &appName, unsigned int appMajorVersion, unsigned int appMinorVersion)
{
    bool res = true;

    // Window initialization
    if (m_window.init("Vulkan", m_width, m_height) == 0) return false;
    // Instance
    if (m_instance.init(appName, m_engineVersionMinor, m_engineVersionMajor, appMajorVersion, appMinorVersion) == 0) return false;
     // Display
    if (m_display.createSurface(m_instance.get(), m_window.window()) == 0) return false;
    // Physical device init
    if (m_physicalDevice.init(m_instance.get(), VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, m_display.surface()) == 0) return false;
    // Logical device
    if (m_logicalDevice.init(m_physicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex(), 1) == 0) return false;
    // Graphics queue
    m_graphicsQueue.init(m_logicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex(), 0);
    m_presentationQueue.init(m_logicalDevice.get(), m_physicalDevice.getPresentationQueueFamilyIndex(), 0);
    // Swap chain
    if (m_display.initSwapchain(m_physicalDevice, m_logicalDevice, m_display, m_width, m_height) == 0) return false;
    // Shaders
    if (m_testVertexShader.init(m_logicalDevice.get(), "./shaders/binaries/triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT) == 0) return false;
    if (m_testFragmentShader.init(m_logicalDevice.get(), "./shaders/binaries/triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT) == 0) return false;
    // Create a render pass
    if (m_renderPass.init(m_logicalDevice.get(), m_display.surfaceFormat().format) == 0) return false;
    // Graphics pipeline
    if (m_graphicsPipeline.init(m_logicalDevice.get(), m_width, m_height, m_testVertexShader, m_testFragmentShader, m_renderPass) == 0) return false;
    // Command pool
    if (m_commandPool.init(m_logicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex()) == 0) return false;
    // Command buffers
    if (m_commandBuffers.init(m_logicalDevice.get(), m_commandPool.get(), m_display.imageCount()) == 0) return false;

    // Success
    return res;
}

void VulkanEngine::mainLoop()
{
    m_window.run();
}

void VulkanEngine::cleanup()
{
    // Shaders
    m_testVertexShader.cleanup(m_logicalDevice.get());
    m_testFragmentShader.cleanup(m_logicalDevice.get());
    // Command pool
    m_commandPool.cleanup(m_logicalDevice.get());
    // Render pass
    m_renderPass.cleanup(m_logicalDevice.get());
    // Graphics pipeline
    m_graphicsPipeline.cleanup(m_logicalDevice.get());
    // Display
    m_display.cleanup(m_logicalDevice.get(), m_instance.get());
    // Logical device
    m_logicalDevice.cleanup();
    // Instance
    m_instance.cleanup();
    // Window
    m_window.cleanup();
}

void VulkanEngine::beginRenderPass(VkCommandBuffer currentCommandBuffer, VkFramebuffer currentFramebuffer)
{
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    // Render pass
    renderPassBeginInfo.renderPass = m_renderPass.get();
    // Color attachment
    renderPassBeginInfo.framebuffer = currentFramebuffer;
    // Size of the render area
    renderPassBeginInfo.renderArea.extent = m_display.surfaceExtent();
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    // Defines the clear value used for the VK_ATTACHMENT_LOAD_OP_CLEAR operation
    renderPassBeginInfo.pClearValues = &m_clearColor;
    renderPassBeginInfo.clearValueCount = 1;

    // Begin render pass
    vkCmdBeginRenderPass(currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanEngine::endRenderPass(VkCommandBuffer currentCommandbuffer)
{
    vkCmdEndRenderPass(currentCommandbuffer);
}

void VulkanEngine::beginDraw(VkCommandBuffer currentCommandbuffer, VkPipeline pipeline)
{
    // Bind the pipline
    vkCmdBindPipeline(currentCommandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    // Send the draw command
    vkCmdDraw(currentCommandbuffer, 3, 1, 0, 0);
}