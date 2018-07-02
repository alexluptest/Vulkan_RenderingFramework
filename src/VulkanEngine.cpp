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

    // Physical device init
    res = m_physicalDevice.init(m_instance.vulkanInstance());
    if (res == false)
        return res;

    // Physical device find queue family
    res = m_physicalDevice.findQueueFamilies(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
    if (res == false)
        return res;

    // Logical device
    res = m_logicalDevice.init(m_physicalDevice.get(), m_physicalDevice.getQueueFamilyIndex(), 1);
    if (res == false)
        return res;

    // Success
    return res;
}

void VulkanEngine::mainLoop()
{
    m_window.run();
}

void VulkanEngine::cleanup()
{
    // Logical device
    m_logicalDevice.cleanup();
    // Instance
    m_instance.cleanup();
    // Window
    m_window.cleanup();
}