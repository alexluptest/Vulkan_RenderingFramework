#include "VulkanApp.h"

#include "VertexFormat.h"

VulkanApp::VulkanApp()
{
    
}

bool VulkanApp::init(uint32_t width, uint32_t height)
{
    // Window initialization
    if (m_window.init("Vulkan", width, height) == 0) return false;

    // Vulkan engine
    m_vulkanEngine.printVersion();
    unsigned int appVersionMajor = 1;
    unsigned int appVersionMinor = 0;
    // Init vulkan engine
    if (m_vulkanEngine.initVulkan(m_window, "VulkanEngine", appVersionMajor, appVersionMinor) == false)
    {
        std::cout << "Failed to init Vulkan. Closing app... \n";
        return false;
    }

    // Shaders
    if (m_quadVertexShader.init(m_vulkanEngine.device(), "./shaders/binaries/triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT) == 0) return false;
    if (m_quadFragmentShader.init(m_vulkanEngine.device(), "./shaders/binaries/triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT) == 0) return false;

    // Create renderable objects
    m_quad = std::make_unique<Quad>(m_vulkanEngine.device());
    if (m_quad->init(m_vulkanEngine,
        width, height,
        { m_quadVertexShader.shaderStageInfo(), m_quadFragmentShader.shaderStageInfo() }) == false)
    {
        std::cout << "Failed to initialize quad.\n";
        return false;
    }

    // Register renderable objects
    m_vulkanEngine.addRenderable(*m_quad);

    // Setup command buffers
    if (m_vulkanEngine.setupCommandBuffers() == 0) return false;

    // Success
    return true;
}

void VulkanApp::update(double dt)
{
    m_quad->update(dt, m_vulkanEngine.frameIndex());
}

void VulkanApp::run()
{
    double dt = 0.0f;

    while (!glfwWindowShouldClose(m_window.get()))
    {
        glfwPollEvents();
        RenderInstance instance(m_vulkanEngine);

        // Rendering
        update(dt);
    }

    // Wait for the queue to finish executing commands before going further
    vkDeviceWaitIdle(m_vulkanEngine.device());
}

void VulkanApp::cleanup()
{
    // Shaders
    m_quadVertexShader.cleanup(m_vulkanEngine.device());
    m_quadFragmentShader.cleanup(m_vulkanEngine.device());

    m_quad->cleanup();

    m_vulkanEngine.cleanup();

    m_window.cleanup();
}