#include "VulkanEngine.h"

#include <string>

bool VulkanEngine::initVulkan(const std::string &appName, unsigned int appMajorVersion, unsigned int appMinorVersion)
{
    bool res = true;

    // Instance
    res = m_instance.init(appName, m_engineVersionMinor, m_engineVersionMajor, appMajorVersion, appMinorVersion);
    if (res == false) 
        return res;

    // Vulkan initialization

    // Success
    return res;
}

void VulkanEngine::mainLoop()
{
}

void VulkanEngine::cleanup()
{
    m_instance.cleanup();
}