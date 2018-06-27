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

    // Success
    return res;
}

void VulkanEngine::mainLoop()
{
    m_window.run();
}

void VulkanEngine::cleanup()
{
    // Instance
    m_instance.cleanup();
    // Window
    m_window.cleanup();
}