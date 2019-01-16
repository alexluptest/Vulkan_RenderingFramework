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
        std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
            {
                0,                              // binding
                sizeof(VertexPC),               // stride
                VK_VERTEX_INPUT_RATE_VERTEX     // inputRate
            }
        };

        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
            // Position attribute - vec2
            {
                0,                          // location
                0,                          // binding
                VK_FORMAT_R32G32_SFLOAT,    // format
                offsetof(VertexPC, pos)     // offset
            },

            // Color attribute - vec3
            {
                1,                          // location
                0,                          // binding
                VK_FORMAT_R32G32B32_SFLOAT, // format
                offsetof(VertexPC, color)   // offset
            }
        };
        
        return attributeDescriptions;
    }
};

#endif // VERTEXFORMAT_H