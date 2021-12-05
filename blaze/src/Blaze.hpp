#pragma once

#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_mem_alloc.hpp>

#include "CommandBuffer.hpp"

struct Input;
struct Display;
struct UserInterface;
struct BlazeInstance;
struct RenderPipeline;

struct RenderContext {

};

struct Blaze {
    struct App {
        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void Update() = 0;
        virtual void Render(CommandBuffer cmd) = 0;
        virtual void Overlay() = 0;
    };

    static auto GetInput() -> Input*;
    static auto GetDisplay() -> Display*;
    static auto GetGraphics() -> RenderPipeline*;

    static auto GetLogicalDevice() -> vk::Device;
    static auto GetPhysicalDevice() -> vk::PhysicalDevice;
    static auto GetMemoryResource() -> VmaAllocator;
    static auto GetPresentQueue() -> vk::Queue;
    static auto GetGraphicsQueue() -> vk::Queue;
    static auto GetPresentFamily() -> uint32_t;
    static auto GetGraphicsFamily() -> uint32_t;
    static auto GetSurface() -> vk::SurfaceKHR;
    static auto GetSupportedDepthFormat() -> vk::Format;

    static auto GetFrameCount() -> size_t;
    static auto GetSurfaceExtent() -> vk::Extent2D;

    static auto GetDeltaTime() -> float;

    static void Start(const std::string& title, int width, int height, bool resizable, App&& app) {
        Start(title, width, height, resizable, app);
    }
    static void Start(const std::string& title, int width, int height, bool resizable, App& app);
};
