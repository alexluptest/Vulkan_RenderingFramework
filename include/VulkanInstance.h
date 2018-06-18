#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

class VulkanInstance
{

public:

    VulkanInstance() {}
    ~VulkanInstance();

    bool init(const std::string &appName, 
        unsigned int major, unsigned int minor,
        unsigned int appMajor, unsigned int appMinor);

private:

    VkInstance m_vkInstance;

};

#endif // VULKANINSTANCE_H