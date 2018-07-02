#include "VulkanQueue.h"

#include <assert.h>

void VulkanQueue::init(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    assert(queueFamilyIndex != -1 && "\nInvalid queue family index.");
    assert(queueFamilyIndex >= 0 && "\nInvalid queue index.");

    vkGetDeviceQueue(logicalDevice, queueFamilyIndex, queueIndex, &m_graphicsQueueHandle);
}