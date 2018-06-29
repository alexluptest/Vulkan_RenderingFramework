#ifndef VULKANHELPER_H
#define VULKANHELPER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkResult createDebugReportCallbackEXT(VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugReportCallbackEXT *pCallback);

void destroyDebugReportCallbackEXT(VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks *pAllocator);

#endif // VULKANHELPER_H