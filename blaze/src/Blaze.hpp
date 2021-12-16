#pragma once

#include <string>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "GraphicsFence.hpp"

struct Blaze {
    struct Engine;
    struct Application {
        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void Update() = 0;
        virtual void Render(CommandBuffer cmd) = 0;
        virtual void DrawUI() = 0;
    };

    static auto GetSwapchainRenderPass() -> vk::RenderPass;

    static auto GetLogicalDevice() -> vk::Device;
    static auto GetMemoryResource() -> VmaAllocator;

    static auto CreateApplication() -> std::unique_ptr<Application>;
};