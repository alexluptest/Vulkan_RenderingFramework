#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include "VulkanHelper.h"
#include <VulkanPhysicalDevice.h>
#include <VulkanQueue.h>

class VulkanBuffer
{

public:

    VulkanBuffer() {}
    ~VulkanBuffer() {}

    bool init(const VulkanPhysicalDevice &physicalDevice, 
        VkDevice logicalDevice,
        size_t elementSize,
        size_t elementCount,
        VkBufferUsageFlags bufferUsage, 
        void *data,
        const VulkanQueue &queue);
    void cleanup(VkDevice device);

    const inline VkBuffer get() const { return m_buffer; }
    const inline uint32_t elementSize() const { return m_elementSize; }
    const inline uint32_t elementCount() const { return m_elementCount; }

private:

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;

    VkBuffer m_stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_stagingBufferMem = VK_NULL_HANDLE;

    uint32_t m_elementSize = 0;
    uint32_t m_elementCount = 0;
    VkDeviceSize m_bufferSize = 0;

    int findMemoryInfo(VkPhysicalDevice physicalDevice, uint32_t memoryType, VkMemoryPropertyFlags properties);
    bool createBuffer(VkPhysicalDevice physicalDevice, 
        VkDevice logicalDevice, 
        size_t bufferSize, 
        VkBufferUsageFlags bufferUsage, 
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkBuffer &buffer,
        VkDeviceMemory &memory);
    bool setStagingBufferData(VkDevice device, size_t bufferSize, void *data);
    void cleanupStagingBuffer(VkDevice device);
    bool copyBuffer(VkDevice device, int transferQueueFamilyIndex, VkDeviceSize size, VkBuffer srcBuffer, VkBuffer dstBuffer, const VulkanQueue &queue);

};

#endif // VULKANBUFFER_H