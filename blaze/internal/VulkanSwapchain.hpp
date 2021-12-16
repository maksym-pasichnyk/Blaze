#pragma once

#include <glm/fwd.hpp>
#include <vulkan/vulkan.hpp>

#include <Texture.hpp>

struct Texture;
struct CommandPool;
struct CommandBuffer;
struct VulkanGfxDevice;

struct VulkanSwapchain {
public:
    explicit VulkanSwapchain(VulkanGfxDevice& gfx);
    ~VulkanSwapchain();

public:
    auto begin(const glm::vec4& color, float depth, glm::u32 stencil) -> CommandBuffer;
    void present();

    [[nodiscard]] auto getRenderPass() const -> vk::RenderPass {
        return _renderPass;
    }
    [[nodiscard]] auto getSurfaceExtent() const -> vk::Extent2D {
        return _surfaceExtent;
    }
    [[nodiscard]] auto getFrameIndex() const -> glm::u32 {
        return _frameIndex;
    }
    [[nodiscard]] auto getFrameCount() const -> size_t {
        return _frameCount;
    }
private:
    void _createSwapchain();
    void _createRenderPass();
    void _createSyncObjects();
    void _createFrameObjects();

private:
    vk::Device _logicalDevice;
    vk::PhysicalDevice _physicalDevice;

    vk::SurfaceKHR _surface;
    vk::Queue _presentQueue;
    vk::Queue _graphicsQueue;

    glm::u32 _presentFamily;
    glm::u32 _graphicsFamily;

    vk::Format _depthFormat;
    vk::RenderPass _renderPass;

    vk::Extent2D _surfaceExtent;
    vk::PresentModeKHR _presentMode;
    vk::SurfaceFormatKHR _surfaceFormat;

    vk::SwapchainKHR _swapchain;

// frame objects
    uint32_t _frameCount = 0;
    uint32_t _frameIndex = 0;
    uint32_t _swapchainImageIndex = 0;

    std::vector<CommandPool> _cmdPools;
    std::vector<CommandBuffer> _cmdBuffers;

    std::vector<vk::Fence> _fences;
    std::vector<vk::Semaphore> _acquiredSemaphores;
    std::vector<vk::Semaphore> _completeSemaphores;

    std::vector<vk::Image> _swapchainImages;
    std::vector<Texture> _swapchainTextures;
    std::vector<Texture2D> _depthTextures;
    std::vector<vk::Framebuffer> _framebuffers;
};

