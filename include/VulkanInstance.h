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

    const inline VkInstance vulkanInstance() const { return m_vkInstance; }

private:

    bool m_enableValidationLayers = true;

    VkInstance m_vkInstance;
    VkDebugReportCallbackEXT m_debugCallback;

    std::vector<VkExtensionProperties> m_supportedExtensions;
    std::vector<VkLayerProperties> m_supportedValidationLayers;

    bool checkValidationLayerSupport(const std::vector<const char*> &requestedValidationLayers);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData
    );

};

#endif // VULKANINSTANCE_H