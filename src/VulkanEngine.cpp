#include "VulkanEngine.h"

#include <string>
#include <limits>

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
    if (m_display.initSwapchain(m_physicalDevice, m_logicalDevice, m_width, m_height) == 0) return false;
    // Shaders
    if (m_testVertexShader.init(m_logicalDevice.get(), "./shaders/binaries/triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT) == 0) return false;
    if (m_testFragmentShader.init(m_logicalDevice.get(), "./shaders/binaries/triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT) == 0) return false;
    // Create a render pass
    if (m_renderPass.init(m_logicalDevice.get(), m_display.surfaceFormat().format) == 0) return false;
    // Create framebuffers for each image view corresponding to each image in the swap chain
    if (m_display.createFramebuffers(m_logicalDevice.get(), m_renderPass.get()) == 0) return false;
    // Graphics pipeline
    if (m_graphicsPipeline.init(m_logicalDevice.get(), m_width, m_height, m_testVertexShader, m_testFragmentShader, m_renderPass) == 0) return false;
    // Command pool
    if (m_commandPool.init(m_logicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex()) == 0) return false;
    // Command buffers
    if (m_commandBuffers.init(m_logicalDevice.get(), m_commandPool.get(), m_display.imageCount()) == 0) return false;
    // Sync objects
    if (m_swapChainSync.init(m_logicalDevice.get(), m_maxFramesInFlight, m_maxFramesInFlight, m_maxFramesInFlight) == 0) return false;  
    // Triangle vertex buffer setup
    std::vector<VertexPC> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    // Init vertex buffer
    if (m_quadVertexBuffer.init(m_physicalDevice, 
            m_logicalDevice.get(), 
            sizeof(vertices[0]),
            vertices.size(),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            reinterpret_cast<void*>(vertices.data()),
            m_graphicsQueue) == 0) return false;
    // Init index buffer
    if (m_quadIndexBuffer.init(m_physicalDevice,
            m_logicalDevice.get(),
            sizeof(indices[0]),
            indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            reinterpret_cast<void*>(indices.data()),
            m_graphicsQueue) == 0) return false;
    // Init uniform buffer
    if (m_quadUniformBuffer.init(m_physicalDevice,
            m_logicalDevice.get(),
            sizeof(UniformBufferObject),
            m_maxFramesInFlight,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            reinterpret_cast<void*>(&m_quadUniformData),
            m_graphicsQueue) == 0) return false;

    // Image
    if (m_testImage.init(m_physicalDevice, 
        m_logicalDevice.get(), 
        VK_IMAGE_TYPE_2D, 
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        300, 300, 1, 1, 1, VK_SAMPLE_COUNT_1_BIT, 
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == false) return false;

    m_testImage.createView(m_logicalDevice.get(), VK_IMAGE_ASPECT_COLOR_BIT);

    // Setup command buffers
    if (setupCommandBuffers() == 0) return false;

    // Success
    return res;
}

void VulkanEngine::render()
{
    // Wait for the current fence frame to be signalled
    if (vkWaitForFences(m_logicalDevice.get(), 1, &m_swapChainSync.fences[m_currentFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
        std::cout << "Failed to wait for the current frame fence object to be signalled. \n";
    if (vkResetFences(m_logicalDevice.get(), 1, &m_swapChainSync.fences[m_currentFrameIndex]) != VK_SUCCESS)
        std::cout << "Failed to reset the current frame fence object. \n";

    // Acquire image from the swap chain - wait for the image to be released by the presentation
    uint32_t availableImageIndex;
    if (vkAcquireNextImageKHR(m_logicalDevice.get(), 
        m_display.swapChain(), 
        std::numeric_limits<uint64_t>::max(),
        m_swapChainSync.waitForObjects[m_currentFrameIndex], // signal this semaphore when the image is acquired
        VK_NULL_HANDLE, 
        &availableImageIndex) != VK_SUCCESS)
    {
        std::cout << "Failed to acquire swap chain image.\n";    
    }

    // Update uniform data
    if (m_quadUniformBuffer.updateUniformData(m_logicalDevice.get(), 
            m_currentFrameIndex,
            reinterpret_cast<void*>(&m_quadUniformData),
            sizeof(UniformBufferObject)) == 0)
        std::cout << "Failed to update uniform data.\n";

    // Prepare a list of command buffers to be executed - we should submit just the command buffer
    // that binds the swap chain image that we just acquired. Execute command buffer 
    // with the current image as attachment - wait for the acquire image
    m_graphicsQueue.submitCommandBuffers(m_swapChainSync, m_currentFrameIndex, { m_commandBuffers.get()[availableImageIndex] });

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
    presentInfo.pImageIndices = &availableImageIndex;
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
        draw(currentCommandBuffer, m_graphicsPipeline.get());
        endRenderPass(currentCommandBuffer);

        // End current command buffer recording
        if (m_commandBuffers.endCommandBuffer(currentCommandBufferIndex) == false)
            return false;
    }

    // Success
    return true;
}

void VulkanEngine::mainLoop()
{
    while (!glfwWindowShouldClose(m_window.window()))
    {
        glfwPollEvents();
        render();
    }

    // Wait for the queue to finish executing commands before going further
    vkDeviceWaitIdle(m_logicalDevice.get());
}

void VulkanEngine::cleanup()
{
    // Shaders
    m_testVertexShader.cleanup(m_logicalDevice.get());
    m_testFragmentShader.cleanup(m_logicalDevice.get());
    // Semaphores
    m_swapChainSync.cleanup(m_logicalDevice.get());
    // Command pool
    m_commandPool.cleanup(m_logicalDevice.get());
    // Render pass
    m_renderPass.cleanup(m_logicalDevice.get());
    // Graphics pipeline
    m_graphicsPipeline.cleanup(m_logicalDevice.get());
    // Display
    m_display.cleanup(m_logicalDevice.get(), m_instance.get());
    // Buffers
    m_quadVertexBuffer.cleanup(m_logicalDevice.get());
    m_quadIndexBuffer.cleanup(m_logicalDevice.get());
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

void VulkanEngine::draw(VkCommandBuffer currentCommandbuffer, VkPipeline pipeline)
{
    // Set dynamic viewport
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = m_width;
    viewport.height = m_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(currentCommandbuffer, 0, 1, &viewport);
    // Bind the pipline
    vkCmdBindPipeline(currentCommandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    // Bind the quad vertex buffer
    VkBuffer vertexBuffers[] = { m_quadVertexBuffer.get() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(currentCommandbuffer, 0, 1, vertexBuffers, offsets);
    // Bind the quad index buffer
    vkCmdBindIndexBuffer(currentCommandbuffer, m_quadIndexBuffer.get(), 0, VK_INDEX_TYPE_UINT16);
    // Send the draw command
    vkCmdDrawIndexed(currentCommandbuffer, m_quadIndexBuffer.elementCount(), 1, 0, 0, 0);
}