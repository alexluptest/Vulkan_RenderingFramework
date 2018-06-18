#include "VulkanInstance.h"

VulkanInstance::~VulkanInstance()
{
    vkDestroyInstance(m_vkInstance, nullptr);
}

bool VulkanInstance::init(const std::string &appName, 
    unsigned int engineMajor, unsigned int engineMinor,
    unsigned int appMajor, unsigned int appMinor)
{
    // Get a list of required extensions by GLFW3
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    // Application info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(appMajor, appMinor, 0);
    appInfo.pEngineName = "VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(engineMajor, engineMinor, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    // Instance info
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = glfwExtensionCount;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.flags = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;

    // Create instance
    VkResult res = vkCreateInstance(&instanceInfo, nullptr, &m_vkInstance);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to create Vulkan instance. \n";
        return false;
    }

    // Success
    return true;
}