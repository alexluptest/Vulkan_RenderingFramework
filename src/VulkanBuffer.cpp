#include "VulkanBuffer.h"
#include <iostream>
#include <cstring>
#include <assert.h>

bool VulkanBuffer::init(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, size_t bufferSize, VkBufferUsageFlags bufferUsage)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    // Create buffer
    if (vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &m_buffer) != VK_SUCCESS)
    {
        std::cout << "Failed to create buffer.\n";
        return false;
    }

    // Get memory requirements
    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(logicalDevice, m_buffer, &memoryRequirements);
    m_bufferSize = memoryRequirements.size;

    // Find memory info

    // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT - used to make sure the contents of the mapped memory 
    // match the contents of the buffer
    int memTypeIndex = findMemoryInfo(physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (memTypeIndex == -1)
        return false;

    // Allocate memory
    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.memoryTypeIndex = memTypeIndex;
    memAllocInfo.allocationSize = memoryRequirements.size;
    if (vkAllocateMemory(logicalDevice, &memAllocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS)
    {
        std::cout << "Failed to allocate buffer memory.\n";
        return false;
    }

    // Bind memory to the buffer
    if (vkBindBufferMemory(logicalDevice, m_buffer, m_bufferMemory, 0) != VK_SUCCESS)
    {
        std::cout << "Failed to bind allocated memory to buffer.\n";
        return false;
    }

    // Success
    return true;
}

bool VulkanBuffer::setData(VkDevice device, void *data)
{
    assert(data != nullptr && "Invalid data pointer.\n");
    assert((size_t)m_bufferSize != 0 && "Buffer not initialized.\n");

    // Bind the allocated buffer memory into CPU accessible memory
    void *cpuMem = nullptr;
    if (vkMapMemory(device, m_bufferMemory, 0, m_bufferSize, 0, &cpuMem) != VK_SUCCESS)
    {
        std::cout << "Failed to map buffer memory to CPU visible memory.\n";
        return false;
    }

    // Copy data to the mapped memory
    memcpy(cpuMem, data, (size_t)m_bufferSize);

    // Unmap the cpu visible memory
    vkUnmapMemory(device, m_bufferMemory);

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