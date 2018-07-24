#include "VulkanQueue.h"

#include <assert.h>
#include <vector>
#include <iostream>

bool VulkanSynchronizationObject::init(VkDevice device, uint32_t waitForObjectCount, uint32_t signalObjectCount, uint32_t fencesCount)
{
    assert((waitForObjectCount != 0 || signalObjectCount != 0 || fencesCount != 0) && "Invalid parameters. At least one count needs to be non-zero.");

    if (waitForObjectCount != 0)
        waitForObjects.resize(waitForObjectCount);
    if (signalObjectCount != 0)
        signalObjects.resize(signalObjectCount);
    if (fencesCount != 0)
        fences.resize(fencesCount);

    // Semaphores
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Wait for objects
    for (auto waitForObjectIndex = 0; waitForObjectIndex < waitForObjectCount; ++waitForObjectIndex)
    {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &waitForObjects[waitForObjectIndex]) != VK_SUCCESS)
        {
            std::cout << "Failed to create wait for semaphore. \n";
            return false;
        }
    }
    
    // Signal objects
    for (auto signalObjectIndex = 0; signalObjectIndex < signalObjectCount; ++signalObjectIndex)
    {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &signalObjects[signalObjectIndex]) != VK_SUCCESS)
        {
            std::cout << "Failed to create signal semaphore. \n";
            return false;
        }
    }

    // Fence objects
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (auto fenceObjectIndex = 0; fenceObjectIndex < fencesCount; ++fenceObjectIndex)
    {
        if (vkCreateFence(device, &fenceCreateInfo, nullptr, &fences[fenceObjectIndex]) != VK_SUCCESS)
        {
            std::cout << "Failed to create fence object. \n";
            return false;
        }
    }

    // Success
    return true;
}

void VulkanSynchronizationObject::cleanup(VkDevice device)
{
    // Wait for objects
    for (auto waitForObject : waitForObjects)
    {
        if (waitForObject != VK_NULL_HANDLE)
            vkDestroySemaphore(device, waitForObject, nullptr);
    }
    
    // Signal objects
    for (auto signalObject : signalObjects)
    {
        if (signalObject != VK_NULL_HANDLE)
            vkDestroySemaphore(device, signalObject, nullptr);
    }

    // Fence object
    for (auto &fenceObject : fences)
    {
        if (fenceObject != VK_NULL_HANDLE)
            vkDestroyFence(device, fenceObject, nullptr);
    }
}

void VulkanQueue::init(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    assert(queueFamilyIndex != -1 && "\nInvalid queue family index.");
    assert(queueFamilyIndex >= 0 && "\nInvalid queue index.");

    vkGetDeviceQueue(logicalDevice, queueFamilyIndex, queueIndex, &m_queueHandle);
}

bool VulkanQueue::submitCommandBuffers(const VulkanSynchronizationObject &syncObject, uint32_t currentFrameIndex, const std::vector<VkCommandBuffer> &commandBuffers)
{
    assert(currentFrameIndex != -1 && "Invalid current frame index");

    // Submit info
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Sync objects
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &syncObject.waitForObjects[currentFrameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &syncObject.signalObjects[currentFrameIndex];

    // Wait before writing color data to the attachment
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitDstStageMask = waitStages;

    // Command buffers
    submitInfo.commandBufferCount = commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();
    
    // Submit command buffers to queue
    // Pass in fence object to be signalled once the command buffer finishes the work
    if (vkQueueSubmit(m_queueHandle, 1, &submitInfo, syncObject.fences[currentFrameIndex]) != VK_SUCCESS)
    {
        std::cout << "Failed to submit command buffers to queue.\n";
        return false;
    }

    // Success
    return true;
}