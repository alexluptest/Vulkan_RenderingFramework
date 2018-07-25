#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include <iostream>
#include <vulkan/vulkan.h>

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanQueue.h"
#include "VulkanDisplay.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"
#include "VulkanBuffer.h"
#include "Window.h"

class VulkanEngine
{
public:

    static VulkanEngine& getInstance()
    {
        static VulkanEngine instance;
        return instance;
    }

    VulkanEngine(const VulkanEngine &other) = delete;
    void operator=(const VulkanEngine &other) = delete;

    bool initVulkan(const std::string &appName, unsigned int appMajorVersion, unsigned int appMinorVersion);
    void mainLoop();
    void printVersion() const { std::cout << "Engine version " << m_engineVersionMajor << "." << m_engineVersionMinor << ".\n"; }
    void cleanup();

private:

    VulkanEngine() {}

    void beginRenderPass(VkCommandBuffer currentCommandBuffer, VkFramebuffer currentFramebuffer);
    void endRenderPass(VkCommandBuffer currentCommandbuffer);
    void draw(VkCommandBuffer currentCommandbuffer, VkPipeline pipeline);
    void render();
    bool setupCommandBuffers();

    unsigned int m_engineVersionMinor = 1;
    unsigned int m_engineVersionMajor = 0;

    uint32_t m_maxFramesInFlight = 2;
    uint32_t m_currentFrameIndex = 0;

    uint32_t m_width = 800, m_height = 600;

    VkClearValue m_clearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; 

    Window m_window;
    VulkanInstance m_instance;
    VulkanPhysicalDevice m_physicalDevice;
    VulkanLogicalDevice m_logicalDevice;
    VulkanQueue m_graphicsQueue, m_presentationQueue;
    VulkanDisplay m_display;
    VulkanGraphicsPipeline m_graphicsPipeline;
    VulkanRenderPass m_renderPass;
    VulkanCommandPool m_commandPool;
    VulkanCommandBuffers m_commandBuffers;
    VulkanBuffer m_triangleVertexBuffer;

    // Synchronization
    VulkanSynchronizationObject m_swapChainSync;

    // Shaders
    VulkanShader m_testVertexShader, m_testFragmentShader;
};

#endif // VULKANENGINE_H