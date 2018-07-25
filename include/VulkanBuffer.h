#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include "VulkanHelper.h"

class VulkanBuffer
{

public:

    VulkanBuffer() {}
    ~VulkanBuffer() {}

    bool init(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, size_t bufferSize, VkBufferUsageFlags bufferUsage);
    bool setData(VkDevice device, void *data);
    void cleanup(VkDevice device);

    const inline VkBuffer get() const { return m_buffer; }

private:

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;
    VkDeviceSize m_bufferSize = 0;

    int findMemoryInfo(VkPhysicalDevice physicalDevice, uint32_t memoryType, VkMemoryPropertyFlags properties);

};

#endif // VULKANBUFFER_H