#ifndef VULKANIMAGE_H
#define VULKANIMAGE_H

#include "VulkanHelper.h"
#include "VulkanPhysicalDevice.h"

struct VulkanImageInfo
{
    VkImageType type;
    VkFormat format;
    VkImageUsageFlags usage;
    VkExtent3D extent;
    uint32_t mipCount;
    uint32_t levelCount;
    VkSampleCountFlagBits samples;
};

class VulkanImage
{

public:

    VulkanImage() = default;
    ~VulkanImage() = default;

    bool init(const VulkanPhysicalDevice &physicalDevice,
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
        VkMemoryPropertyFlags memoryPropertyFlags);
    void cleanup(VkDevice device);
    void transitionLayoutTo(VkImageLayout newLayout);
    bool createView(VkDevice device,
        VkImageAspectFlags imageAspect,
        VkFormat viewFormat = VK_FORMAT_UNDEFINED,
        VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM,
        uint32_t baseArrayLevel = 0,
        uint32_t layerCount = 1,
        uint32_t baseMipLevel = 0,
        uint32_t levelCount = 1);

    // Accessors
    const inline VkImage get() const { return m_image; }

private:

    bool createImage(VkDevice device);
    bool allocateImageMemory(const VulkanPhysicalDevice &physicalDevice, VkDevice device, VkMemoryPropertyFlags memoryPropertyFlags);

    VulkanImageInfo m_imageInfo = {};
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageLayout m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED, m_oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;

};

#endif // VULKANIMAGE_H