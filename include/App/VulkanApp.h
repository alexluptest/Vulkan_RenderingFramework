#ifndef VULKANAPP_H
#define VULKANAPP_H

#include <memory>

#include "VulkanHelper.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanEngine.h"
#include "Window.h"

#include "Quad.h"

class VulkanApp
{

public:

    static VulkanApp& getInstance()
    {
        static VulkanApp instance;
        return instance;
    }

    VulkanApp(const VulkanApp &other) = delete;
    void operator=(const VulkanApp &other) = delete;

    bool init(uint32_t width, uint32_t height);
    void cleanup();
    void run();

private:

    VulkanApp();

    void update(double dt);
    void mainLoop();

    Window m_window;

    VulkanEngine &m_vulkanEngine = VulkanEngine::getInstance();
    VulkanShader m_quadVertexShader, m_quadFragmentShader;

    std::unique_ptr<Quad> m_quad;
};

#endif // VULKANAPP_H