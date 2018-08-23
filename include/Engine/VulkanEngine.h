#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include <iostream>
#include <vector>
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
#include "VulkanImage.h"
#include "Window.h"
#include "VulkanRenderableObject.h"

class VulkanEngine
{
public:

    friend class RenderInstance;

    static VulkanEngine& getInstance()
    {
        static VulkanEngine instance;
        return instance;
    }

    VulkanEngine(const VulkanEngine &other) = delete;
    void operator=(const VulkanEngine &other) = delete;

    bool initVulkan(const Window &window, const std::string &appName, unsigned int appMajorVersion, unsigned int appMinorVersion);
    void mainLoop();
    void printVersion() const { std::cout << "Engine version " << m_engineVersionMajor << "." << m_engineVersionMinor << ".\n"; }
    void cleanup();
    bool setupCommandBuffers();
    const inline void addRenderable(const VulkanRenderableObject &object) { m_renderableList.push_back(&object); }

    const inline VkDevice device() const { return m_logicalDevice.get(); }
    const inline VulkanPhysicalDevice &physicalDevice() const { return m_physicalDevice; }
    const inline VulkanDisplay &display() const { return m_display; }
    const inline VulkanRenderPass &renderPass() const { return m_renderPass; }
    const inline VulkanQueue &graphicsQueue() const { return m_graphicsQueue; }
    const inline uint32_t framesInFlight() const { return m_maxFramesInFlight; }
    const inline uint32_t frameIndex() const { return m_currentFrameIndex; }

private:

    VulkanEngine() {}

    void beginRenderPass(VkCommandBuffer currentCommandBuffer, VkFramebuffer currentFramebuffer);
    void endRenderPass(VkCommandBuffer currentCommandbuffer);

    void beginRender();
    void endRender();

    unsigned int m_engineVersionMinor = 1;
    unsigned int m_engineVersionMajor = 0;

    uint32_t m_maxFramesInFlight = 2;
    uint32_t m_currentFrameIndex = 0;
    uint32_t m_availableImageIndex = 0;

    VkClearValue m_clearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; 

    VulkanInstance m_instance;
    VulkanPhysicalDevice m_physicalDevice;
    VulkanLogicalDevice m_logicalDevice;
    VulkanQueue m_graphicsQueue, m_presentationQueue;
    VulkanDisplay m_display;
    VulkanRenderPass m_renderPass;
    VulkanCommandPool m_commandPool;
    VulkanCommandBuffers m_commandBuffers;

    // Synchronization
    VulkanSynchronizationObject m_swapChainSync;

    // Renderable objects
    std::vector<const VulkanRenderableObject*> m_renderableList;
};

class RenderInstance
{

public:

    RenderInstance(VulkanEngine &engine);
    ~RenderInstance();

private:

    VulkanEngine &m_vulkanEngine;

};

#endif // VULKANENGINE_H