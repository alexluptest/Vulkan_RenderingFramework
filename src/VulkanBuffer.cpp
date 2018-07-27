#include "VulkanBuffer.h"
#include <iostream>
#include <cstring>
#include <assert.h>
#include <VulkanCommandPool.h>
#include <VulkanCommandBuffers.h>

bool VulkanBuffer::init(const VulkanPhysicalDevice &physicalDevice, 
    VkDevice logicalDevice, 
    size_t bufferSize, 
    VkBufferUsageFlags bufferUsage, 
    VkMemoryPropertyFlags memoryPropertyFlags,
    void *data,
    const VulkanQueue &queue)
{
    assert(data != nullptr && "Invalid data to set in the buffer");

    // Create staging buffer
    if (createBuffer(physicalDevice.get(), 
        logicalDevice, 
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        m_stagingBuffer,
        m_stagingBufferMem) == 0)
        return false;

    // Copy data to the staging buffer
    if (setStagingBufferData(logicalDevice, bufferSize, data) == 0)
        return false;

    // Create device buffer - use transfer dest flag so we can copy data from the staging buffer
    if (createBuffer(physicalDevice.get(), 
        logicalDevice,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_buffer,
        m_bufferMemory) == 0)
        return false;

    // Copy data from the staging buffer to the device buffer
    if (copyBuffer(logicalDevice, physicalDevice.getGraphicsQueueFamilyIndex(), bufferSize, m_stagingBuffer, m_buffer, queue) == 0)
        return false;

    // Clean staging buffer
    cleanupStagingBuffer(logicalDevice);

    // Success
    return true;
}

bool VulkanBuffer::setStagingBufferData(VkDevice device, size_t bufferSize, void *data)
{
    assert(data != nullptr && "Invalid data pointer.\n");
    assert((size_t)bufferSize != 0 && "Buffer not initialized.\n");

    // Bind the allocated buffer memory into CPU accessible memory (staging buffer)
    void *cpuMem = nullptr;
    if (vkMapMemory(device, m_stagingBufferMem, 0, bufferSize, 0, &cpuMem) != VK_SUCCESS)
    {
        std::cout << "Failed to map buffer memory to CPU visible memory.\n";
        return false;
    }
    // Copy data to the mapped memory
    memcpy(cpuMem, data, (size_t)bufferSize);
    // Unmap the cpu visible memory
    vkUnmapMemory(device, m_stagingBufferMem);

    // Success
    return true;
}

void VulkanBuffer::cleanup(VkDevice device)
{
    if (m_buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(device, m_buffer, nullptr);
    if (m_bufferMemory != VK_NULL_HANDLE)
        vkFreeMemory(device, m_bufferMemory, nullptr);
}

void VulkanBuffer::cleanupStagingBuffer(VkDevice device)
{
    if (m_stagingBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(device, m_stagingBuffer, nullptr);
    if (m_stagingBufferMem != VK_NULL_HANDLE)
        vkFreeMemory(device, m_stagingBufferMem, nullptr);
}

int VulkanBuffer::findMemoryInfo(VkPhysicalDevice physicalDevice, uint32_t memoryType, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (int memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; ++memTypeIndex)
    {
        if (memoryType & (1 << memTypeIndex) &&
            (memProperties.memoryTypes[memTypeIndex].propertyFlags & properties) == properties)
            return memTypeIndex;
    }

    // Fail
    std::cout << "We failed to find a memory type that satisfies the type of memory and the memory properties. \n";
    return -1;
}

bool VulkanBuffer::createBuffer(VkPhysicalDevice physicalDevice, 
    VkDevice logicalDevice, 
    size_t bufferSize, 
    VkBufferUsageFlags bufferUsage, 
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkBuffer &buffer,
    VkDeviceMemory &memory)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    // Create buffer
    if (vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        std::cout << "Failed to create buffer.\n";
        return false;
    }

    // Get memory requirements
    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);
    m_bufferSize = memoryRequirements.size;

    // Find memory info

    // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT - used to make sure the contents of the mapped memory 
    // match the contents of the buffer
    int memTypeIndex = findMemoryInfo(physicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags);
    if (memTypeIndex == -1)
        return false;

    // Allocate memory
    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.memoryTypeIndex = memTypeIndex;
    memAllocInfo.allocationSize = memoryRequirements.size;
    if (vkAllocateMemory(logicalDevice, &memAllocInfo, nullptr, &memory) != VK_SUCCESS)
    {
        std::cout << "Failed to allocate buffer memory.\n";
        return false;
    }

    // Bind memory to the buffer
    if (vkBindBufferMemory(logicalDevice, buffer, memory, 0) != VK_SUCCESS)
    {
        std::cout << "Failed to bind allocated memory to buffer.\n";
        return false;
    }

    // Success
    return true;
}

bool VulkanBuffer::copyBuffer(VkDevice device,
    int transferQueueFamilyIndex,
    VkDeviceSize size, 
    VkBuffer srcBuffer, 
    VkBuffer dstBuffer,
    const VulkanQueue &queue)
{
    assert(transferQueueFamilyIndex != -1 && "Invalid transfer queue family index.");

    VulkanCommandPool tempCommandPool;
    VulkanCommandBuffers tempCommandBuffers;

    // Command pool
    if (tempCommandPool.init(device, transferQueueFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) == 0) return false;
    // Command buffers
    if (tempCommandBuffers.init(device, tempCommandPool.get(), 1) == 0) return false;

    // Begin command buffer recording
    if (tempCommandBuffers.beginCommandBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) == 0)
        return false;
    
    // Submit copy command to the newly create command buffer
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(tempCommandBuffers.get()[0], srcBuffer, dstBuffer, 1, &copyRegion);

    // End command buffer recording
    if (tempCommandBuffers.endCommandBuffer(0) == 0)
        return false;

    // Execute command buffer
    queue.submitCommandBuffers(device, tempCommandBuffers.get());

    // Success
    return true;
}