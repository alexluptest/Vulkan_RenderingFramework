#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <vector>

class VulkanInstance
{

public:

    VulkanInstance() {}
    ~VulkanInstance();

    bool init(const std::string &appName, 
        unsigned int major, unsigned int minor,
        unsigned int appMajor, unsigned int appMinor);
    void cleanup();

private:

    VkInstance m_vkInstance;

    std::vector<VkExtensionProperties> m_supportedExtensions;

};

#endif // VULKANINSTANCE_H