#include "VulkanEngine.h"

#include "Window.h"

#include <string>
#include <limits>

bool VulkanEngine::initVulkan(const Window &window, const std::string &appName, unsigned int appMajorVersion, unsigned int appMinorVersion)
{
    bool res = true;

    // Instance
    if (m_instance.init(appName, m_engineVersionMinor, m_engineVersionMajor, appMajorVersion, appMinorVersion) == 0) return false;
     // Display
    if (m_display.createSurface(m_instance.get(), window.get()) == 0) return false;
    // Physical device init
    if (m_physicalDevice.init(m_instance.get(), VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, m_display.surface()) == 0) return false;
    // Logical device
    if (m_logicalDevice.init(m_physicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex(), 1) == 0) return false;
    // Graphics queue
    m_graphicsQueue.init(m_logicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex(), 0);
    m_presentationQueue.init(m_logicalDevice.get(), m_physicalDevice.getPresentationQueueFamilyIndex(), 0);
    // Swap chain
    if (m_display.initSwapchain(m_physicalDevice, m_logicalDevice, window.width(), window.height()) == 0) return false;
    // Create a render pass
    if (m_renderPass.init(m_logicalDevice.get(), m_display.surfaceFormat().format) == 0) return false;
    // Create framebuffers for each image view corresponding to each image in the swap chain
    if (m_display.createFramebuffers(m_logicalDevice.get(), m_renderPass.get()) == 0) return false;
    // Command pool
    if (m_commandPool.init(m_logicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex()) == 0) return false;
    // Command buffers
    if (m_commandBuffers.init(m_logicalDevice.get(), m_commandPool.get(), m_display.imageCount()) == 0) return false;
    // Sync objects
    if (m_swapChainSync.init(m_logicalDevice.get(), m_maxFramesInFlight, m_maxFramesInFlight, m_maxFramesInFlight) == 0) return false;
    // Setup command buffers
    if (setupCommandBuffers() == 0) return false;

    // Success
    return res;
}

void VulkanEngine::beginRender()
{
    // Wait for the current fence frame to be signalled
    if (vkWaitForFences(m_logicalDevice.get(), 1, &m_swapChainSync.fences[m_currentFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
        std::cout << "Failed to wait for the current frame fence object to be signalled. \n";
    if (vkResetFences(m_logicalDevice.get(), 1, &m_swapChainSync.fences[m_currentFrameIndex]) != VK_SUCCESS)
        std::cout << "Failed to reset the current frame fence object. \n";

    // Acquire image from the swap chain - wait for the image to be released by the presentation
    if (vkAcquireNextImageKHR(m_logicalDevice.get(), 
        m_display.swapChain(), 
        std::numeric_limits<uint64_t>::max(),
        m_swapChainSync.waitForObjects[m_currentFrameIndex], // signal this semaphore when the image is acquired
        VK_NULL_HANDLE, 
        &m_availableImageIndex) != VK_SUCCESS)
    {
        std::cout << "Failed to acquire swap chain image.\n";    
    }
}
    
void VulkanEngine::endRender()
{
    // Prepare a list of command buffers to be executed - we should submit just the command buffer
    // that binds the swap chain image that we just acquired. Execute command buffer 
    // with the current image as attachment - wait for the acquire image
    m_graphicsQueue.submitCommandBuffers(m_swapChainSync, m_currentFrameIndex, { m_commandBuffers.get()[m_availableImageIndex] });

    // Do the presentation
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    // Specifies which semaphore to wait for (which is signalled when the command buffer is finished executing)
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_swapChainSync.signalObjects[m_currentFrameIndex];
    // Specifies the swap chain to present images to and the image index for each one
    presentInfo.swapchainCount = 1;
    VkSwapchainKHR swapChains[] = { m_display.swapChain() };
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_availableImageIndex;
    presentInfo.pResults = nullptr;

    // Send presentation commands to the presentation queue
    if (vkQueuePresentKHR(m_presentationQueue.queueHandle(), &presentInfo) != VK_SUCCESS)
    {
        std::cout << "Failed to send the presentation request.\n";
    }

    // Update current frame index
    m_currentFrameIndex = (m_currentFrameIndex + 1) % m_maxFramesInFlight;
}

bool VulkanEngine::setupCommandBuffers()
{
    auto &commandBuffers = m_commandBuffers.get();

    for (auto &currentCommandBuffer : commandBuffers)
    {
        int currentCommandBufferIndex = &currentCommandBuffer - commandBuffers.data();

        // Begin current command buffer recording
        if (m_commandBuffers.beginCommandBuffer(currentCommandBufferIndex) == false)
            return false;

        beginRenderPass(currentCommandBuffer, m_display.framebuffer(currentCommandBufferIndex));
        
        for (auto &renderableObject : m_renderableList)
        {
            renderableObject->render(currentCommandBuffer);
        }
        endRenderPass(currentCommandBuffer);

        // End current command buffer recording
        if (m_commandBuffers.endCommandBuffer(currentCommandBufferIndex) == false)
            return false;
    }

    // Success
    return true;
}

void VulkanEngine::cleanup()
{
    // Semaphores
    m_swapChainSync.cleanup(m_logicalDevice.get());
    // Command pool
    m_commandPool.cleanup(m_logicalDevice.get());
    // Render pass
    m_renderPass.cleanup(m_logicalDevice.get());
    // Display
    m_display.cleanup(m_logicalDevice.get(), m_instance.get());
    // Logical device
    m_logicalDevice.cleanup();
    // Instance
    m_instance.cleanup();
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

RenderInstance::RenderInstance(VulkanEngine &engine)
    : m_vulkanEngine(engine)
{
    m_vulkanEngine.beginRender();
}

RenderInstance::~RenderInstance()
{
    m_vulkanEngine.endRender();
}