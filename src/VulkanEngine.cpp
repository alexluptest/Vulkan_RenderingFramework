#include "VulkanEngine.h"

#include <string>

bool VulkanEngine::initVulkan(const std::string &appName, unsigned int appMajorVersion, unsigned int appMinorVersion)
{
    bool res = true;

    // Window initialization
    res = m_window.init("Vulkan", 800, 600);
    if (res == false)
        return res;

    // Instance
    res = m_instance.init(appName, m_engineVersionMinor, m_engineVersionMajor, appMajorVersion, appMinorVersion);
    if (res == false) 
        return res;

    // Display
    res = m_display.createSurface(m_instance.get(), m_window.window());
    if (res == false)
        return res;

    // Physical device init
    res = m_physicalDevice.init(m_instance.get(), VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, m_display.surface());
    if (res == false)
        return res;

    // Logical device
    res = m_logicalDevice.init(m_physicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex(), 1);
    if (res == false)
        return res;

    // Graphics queue
    m_graphicsQueue.init(m_logicalDevice.get(), m_physicalDevice.getGraphicsQueueFamilyIndex(), 0);


    // Success
    return res;
}

void VulkanEngine::mainLoop()
{
    m_window.run();
}

void VulkanEngine::cleanup()
{
    // Display
    m_display.cleanup(m_instance.get());
    // Logical device
    m_logicalDevice.cleanup();
    // Instance
    m_instance.cleanup();
    // Window
    m_window.cleanup();
}