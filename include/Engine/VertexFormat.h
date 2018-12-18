#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

#include "VulkanHelper.h"
#include <glm/glm.hpp>
#include <array>

struct VertexPC
{
    glm::vec2 pos;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> getBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(VertexPC);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        
        // Position attribute - vec2
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].offset = offsetof(VertexPC, pos);

        // Color attribute - vec3
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].offset = offsetof(VertexPC, color);
        
        return attributeDescriptions;
    }
};

#endif // VERTEXFORMAT_H