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
    bool updateUniformData(VkDevice device, uint32_t currentImage, void *data, size_t dataSize);
    void cleanup(VkDevice device);

    const inline VkBuffer get() const { return m_buffers[0]; }
    const inline uint32_t elementSize() const { return m_elementSize; }
    const inline uint32_t elementCount() const { return m_elementCount; }

private:

    std::vector<VkBuffer> m_buffers;
    std::vector<VkDeviceMemory> m_memoryBuffers;

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
    bool createVertexBuffer(const VulkanPhysicalDevice &physicalDevice, 
        VkDevice logicalDevice,
        size_t elementSize,
        size_t elementCount,
        VkBufferUsageFlags bufferUsage, 
        void *data,
        const VulkanQueue &queue);
    bool createUniformBuffer(const VulkanPhysicalDevice &physicalDevice, 
        VkDevice logicalDevice,
        size_t elementSize,
        size_t elementCount,
        VkBufferUsageFlags bufferUsage, 
        void *data,
        const VulkanQueue &queue);
    bool setStagingBufferData(VkDevice device, size_t bufferSize, void *data);
    void cleanupStagingBuffer(VkDevice device);
    bool copyBuffer(VkDevice device, int transferQueueFamilyIndex, VkDeviceSize size, VkBuffer srcBuffer, VkBuffer dstBuffer, const VulkanQueue &queue);

};

#endif // VULKANBUFFER_H