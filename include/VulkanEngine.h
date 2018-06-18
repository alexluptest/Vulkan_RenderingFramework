#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include <iostream>
#include <vulkan/vulkan.h>

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

    void initVulkan(const std::string &appName);
    void mainLoop();
    void printVersion() const { std::cout << "Engine version " << m_engineVersionMajor << "." << m_engineVersionMinor << ".\n"; }
    void cleanup();

private:

    VulkanEngine() {}

    unsigned int m_engineVersionMinor = 1;
    unsigned int m_engineVersionMajor = 0;

};

#endif // VULKANENGINE_H