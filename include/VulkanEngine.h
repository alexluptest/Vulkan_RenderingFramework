#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include <iostream>
#include <vulkan/vulkan.h>

#include "VulkanInstance.h"
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

    unsigned int m_engineVersionMinor = 1;
    unsigned int m_engineVersionMajor = 0;

    Window m_window;
    VulkanInstance m_instance;

};

#endif // VULKANENGINE_H