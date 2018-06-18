#include<iostream>

#include "VulkanEngine.h"
 
int main(int argc, char const *argv[])
{
    
    unsigned int appVersionMajor = 1;
    unsigned int appVersionMinor = 0;

    auto &vulkanEngine = VulkanEngine::getInstance();
    vulkanEngine.printVersion();

    // Init vulkan
    bool res = vulkanEngine.initVulkan("VulkanEngine", appVersionMajor, appVersionMinor);
    if (res == false)
    {
        std::cout << "Failed to init Vulkan. Closing app... \n";
        return -1;
    }

    return 0;
}