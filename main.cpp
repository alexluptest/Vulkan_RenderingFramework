#include<iostream>

#include "VulkanEngine.h"
#include "VulkanApp.h"
 
int main(int argc, char const *argv[])
{
    uint32_t m_width = 800, m_height = 600;

    auto &vulkanApp = VulkanApp::getInstance();
    if (vulkanApp.init(m_width, m_height) == false)
    {
        std::cout << "Failed to initialize vulkan app.\n";
        return -1;
    }

    // Main loop
    vulkanApp.run();

    // Cleanup vulkan
    vulkanApp.cleanup();

    return 0;
}