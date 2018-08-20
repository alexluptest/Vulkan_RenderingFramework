#include "VulkanImage.h"

#include <assert.h>
#include <iostream>

bool VulkanImage::init(const VulkanPhysicalDevice &physicalDevice,
    VkDevice device, 
    VkImageType imageType, 
    VkFormat format,
    VkImageUsageFlags imageUsage,
    uint32_t width,
    uint32_t height,
    uint32_t depth,
    uint32_t mipCount,
    uint32_t levelCount,
    VkSampleCountFlagBits sampleCount,
    VkImageLayout initialLayout,
    VkMemoryPropertyFlags memoryPropertyFlags)
{
    assert(width > 0 && "Invalid image width.");
    assert(height > 0 && "Invalid image height.");

    // Store image information
    m_imageInfo.type = imageType;
    m_imageInfo.format = format;
    m_imageInfo.usage = imageUsage;
    m_imageInfo.extent = { width, height, depth };
    m_imageInfo.mipCount = mipCount;
    m_imageInfo.levelCount = levelCount;
    m_imageInfo.samples = sampleCount;

    // Initialze initial layout
    m_currentLayout = initialLayout;

    // Create vulkan image
    if (createImage(device) == false)
        return false;

    // Allocate memory for the created image
    if (allocateImageMemory(physicalDevice, device, memoryPropertyFlags) == false)
        return false;

    // Bind the allocated memory to our image handle
    if (vkBindImageMemory(device, m_image, m_imageMemory, 0) != VK_SUCCESS)
    {
        std::cout << "Failed to bind memory to our image handle. \n";
        return false;
    }

    // Success
    return true;
}
    
void VulkanImage::cleanup(VkDevice device)
{
    if (m_image != VK_NULL_HANDLE)
        vkDestroyImage(device, m_image, nullptr);
    if (m_imageView != VK_NULL_HANDLE)
        vkDestroyImageView(device, m_imageView, nullptr);
}

void VulkanImage::transitionLayoutTo(VkImageLayout newLayout)
{
    m_oldLayout = m_currentLayout;
    m_currentLayout = newLayout;

    // TODO do layout transition
}

bool VulkanImage::createView(VkDevice device,
    VkImageAspectFlags imageAspect,
    VkFormat viewFormat,
    VkImageViewType imageViewType,
    uint32_t baseArrayLevel,
    uint32_t layerCount,
    uint32_t baseMipLevel,
    uint32_t levelCount)
{
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_image;
    imageViewCreateInfo.format = (viewFormat == VK_FORMAT_UNDEFINED) ? m_imageInfo.format : viewFormat;
    imageViewCreateInfo.flags = 0;
    
    // Image view type
    VkImageViewType viewType = imageViewType;
    if (viewType == VK_IMAGE_VIEW_TYPE_MAX_ENUM)
    {
        switch (m_imageInfo.type)
        {
            case VK_IMAGE_TYPE_1D:
            {
                if (m_imageInfo.levelCount > 1)
                    viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                else
                    viewType = VK_IMAGE_VIEW_TYPE_1D;
                break;
            }
            case VK_IMAGE_TYPE_2D:
            {
                if (m_imageInfo.levelCount > 1)
                    viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                else
                    viewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            }                    
            case VK_IMAGE_TYPE_3D:
                viewType = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                std::cout << "Invalid image type. Can't decide on the image view type.\n";
        }
    }
    imageViewCreateInfo.viewType = viewType;

    // Image component mapping
    VkComponentMapping componentMapping = {};
    componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components = componentMapping;

    // Image subresource range
    VkImageSubresourceRange imageSubresourceRange = {};
    imageSubresourceRange.aspectMask = imageAspect;
    imageSubresourceRange.baseArrayLayer = baseArrayLevel;
    imageSubresourceRange.layerCount = layerCount;
    imageSubresourceRange.baseMipLevel = baseMipLevel;
    imageSubresourceRange.levelCount = levelCount;
    imageViewCreateInfo.subresourceRange = imageSubresourceRange;

    if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_imageView) != VK_SUCCESS)
    {
        std::cout << "Failed to create image view. \n";
        return false;
    }

    // Success
    return true;
}

bool VulkanImage::createImage(VkDevice device)
{
    // Image create info
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.flags = (m_imageInfo.type == VK_IMAGE_TYPE_3D) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
    imageCreateInfo.imageType = m_imageInfo.type;
    imageCreateInfo.format = m_imageInfo.format;
    imageCreateInfo.extent = { m_imageInfo.extent.width,                            // Width
        m_imageInfo.extent.height,                                                  // Height
        (m_imageInfo.type == VK_IMAGE_TYPE_3D) ? m_imageInfo.extent.depth : 1 };    // Depth
    imageCreateInfo.mipLevels = m_imageInfo.mipCount;
    imageCreateInfo.arrayLayers = m_imageInfo.levelCount;
    imageCreateInfo.samples = m_imageInfo.samples;
    // Use optimal tiling - we don't know how the image memory layout is going to be
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    // Image usage
    imageCreateInfo.usage = m_imageInfo.usage;
    // Image is used by queues created from the same queue family
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.initialLayout = m_currentLayout;

    // Create image
    if (vkCreateImage(device, &imageCreateInfo, nullptr, &m_image) != VK_SUCCESS)
    {
        std::cout << "Failed to create image. \n";
        return false;
    }

    // Success
    return true;
}

bool VulkanImage::allocateImageMemory(const VulkanPhysicalDevice &physicalDevice, VkDevice device, VkMemoryPropertyFlags memoryPropertyFlags)
{
    // Get image memory requirements

    // Size, alignment, memory types allowed
    VkMemoryRequirements imageMemRequirements = {};
    vkGetImageMemoryRequirements(device, m_image, &imageMemRequirements);

    auto &supportedMemoryTypes = physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < supportedMemoryTypes.memoryTypeCount; ++i)
    {
        bool memTypeSupported = imageMemRequirements.memoryTypeBits & (1 << i);
        bool memTypeHasRequiredProperties = supportedMemoryTypes.memoryTypes[i].propertyFlags & memoryPropertyFlags;
        if (memTypeSupported && memTypeHasRequiredProperties)
        {
            // We found a suitable memory type
            
            // Fill the memory allocate information
            VkMemoryAllocateInfo memoryAllocateInfo = {};
            memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memoryAllocateInfo.memoryTypeIndex = i;
            memoryAllocateInfo.allocationSize = imageMemRequirements.size;

            // Allocate image memory
            if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_imageMemory) != VK_SUCCESS)
            {
                std::cout << "Failed to allocate image memory.\n";
                return false;
            }
            else
                break;
        }
    }

    // Check if we managed to allocate some image memory, if not stop
    if (m_imageMemory == VK_NULL_HANDLE)
    {
        std::cout << "Failed to find a suitable memory type to use as the image memory.\n";
        return false;
    }

    // Success
    return true;
}