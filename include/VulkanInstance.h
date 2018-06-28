#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <vector>
#include <cstring>

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

    bool m_enableValidationLayers = true;

    VkInstance m_vkInstance;

    std::vector<VkExtensionProperties> m_supportedExtensions;
    std::vector<VkLayerProperties> m_supportedValidationLayers;

    bool checkValidationLayerSupport(const std::vector<const char*> &requestedValidationLayers);

};

#endif // VULKANINSTANCE_H