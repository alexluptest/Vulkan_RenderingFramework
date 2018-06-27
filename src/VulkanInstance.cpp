#include "VulkanInstance.h"

VulkanInstance::~VulkanInstance() {}

bool VulkanInstance::init(const std::string &appName, 
    unsigned int engineMajor, unsigned int engineMinor,
    unsigned int appMajor, unsigned int appMinor)
{
    VkResult res = VK_SUCCESS;

    // Get a list of required extensions by GLFW3
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Get a list of supported extensions

    // Get extension count
    uint32_t extensionCount = 0;
    res = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get supported extension count. \n";
        return false;
    }
    // Get the list of supported extensions
    m_supportedExtensions.resize(extensionCount);
    res = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_supportedExtensions.data());
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to enumerate the supported extensions. \n";
        return false;
    }
    // Print list of supported extensions
    std::cout << "Supported extensions: \n";
    for (auto &extension : m_supportedExtensions)
    {
        std::cout << extension.extensionName << "\n";
    }

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
    res = vkCreateInstance(&instanceInfo, nullptr, &m_vkInstance);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to create Vulkan instance. \n";
        return false;
    }

    // Success
    return true;
}

void VulkanInstance::cleanup()
{
    vkDestroyInstance(m_vkInstance, nullptr);
}