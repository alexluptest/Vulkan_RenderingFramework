#include "VulkanInstance.h"

#include "VulkanHelper.h"

VulkanInstance::~VulkanInstance() {}

bool VulkanInstance::init(const std::string &appName, 
    unsigned int engineMajor, unsigned int engineMinor,
    unsigned int appMajor, unsigned int appMinor)
{
    VkResult res = VK_SUCCESS;

    // Initialize a list of validation layers to enable
    std::vector<const char*> validationLayers;
    if (m_enableValidationLayers)
        validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
    if (m_enableValidationLayers)
    {
        if (checkValidationLayerSupport(validationLayers) == false)
        {
            std::cout << "Failed to find all the requested validation layers.\n";
            return false;
        }
    }

    // Get a list of required extensions by GLFW3
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    // Build a list of required extensions
    std::vector<const char*> requestedExtensions;
    for (int i = 0; i < glfwExtensionCount; ++i)
        requestedExtensions.push_back(glfwExtensions[i]);
    if (m_enableValidationLayers)
        requestedExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

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
    std::cout << "\nSupported extensions: \n\n";
    for (auto &extension : m_supportedExtensions)
    {
        std::cout << extension.extensionName << "\n";
    }

    // Check if all the requested extensions are available
    // TODO

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
    instanceInfo.enabledExtensionCount = requestedExtensions.size();
    instanceInfo.ppEnabledExtensionNames = requestedExtensions.data();
    if (m_enableValidationLayers)
    {
        instanceInfo.enabledLayerCount = validationLayers.size();
        instanceInfo.ppEnabledLayerNames = validationLayers.data();
    }
    instanceInfo.flags = 0;

    // Create instance
    res = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to create Vulkan instance. \n";
        return false;
    }

    // Setup debug callback
    if (m_enableValidationLayers)
    {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        // Filter which types of messages to receive
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        // Set callback function
        createInfo.pfnCallback = VulkanInstance::debugCallback;

        res = createDebugReportCallbackEXT(m_instance, &createInfo, nullptr, &m_debugCallback);
        if (res != VK_SUCCESS)
        {
            std::cout << "Failed to create the debug callback.\n";
            return false;
        }
    }

    // Success
    return true;
}

void VulkanInstance::cleanup()
{
    if (m_enableValidationLayers)
        destroyDebugReportCallbackEXT(m_instance, m_debugCallback, nullptr);

    if (m_instance != VK_NULL_HANDLE)
        vkDestroyInstance(m_instance, nullptr);
}

bool VulkanInstance::checkValidationLayerSupport(const std::vector<const char*> &requestedValidationLayers)
{
    VkResult res;

    // Get validation layer count
    uint32_t layerCount = 0;
    res = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get validation layer count. \n";
        return false;
    }

    if (layerCount != 0)
        m_supportedValidationLayers.resize(layerCount);

    // Get available validation layers
    res = vkEnumerateInstanceLayerProperties(&layerCount, m_supportedValidationLayers.data());
    if (res != VK_SUCCESS)
    {
        std::cout << "Failed to get the available validation layers. \n";
        return false;
    }

    // Print available validation layers
    std::cout << "\nValidation layers: \n\n";
    for (auto &validationLayer : m_supportedValidationLayers)
        std::cout << validationLayer.layerName << "\n";

    // Check if all the requested validation layers are available
    for (auto &requestedLayer : requestedValidationLayers)
    {
        bool layerFound = false;
        for (auto &validationLayer : m_supportedValidationLayers)
        {
            if (std::strcmp(requestedLayer, validationLayer.layerName) == 0)
            {
                layerFound = true;
                break;
            }            
        }

        if (layerFound == false)
        {
            std::cout << "Validation layer " << requestedLayer << " is not available. \n";
            return false;
        }
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::debugCallback(VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData)
{
    std::cerr << "validation layer: " << msg << std::endl;
    return VK_FALSE;
}