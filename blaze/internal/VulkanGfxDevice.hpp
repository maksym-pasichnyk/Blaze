#pragma once

#include "VulkanSwapchain.hpp"

#include <Graphics.hpp>
#include <GraphicsBuffer.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_beta.h>

struct VulkanGfxDevice {
public:
    explicit VulkanGfxDevice(Display& display);
    ~VulkanGfxDevice();

    [[nodiscard]] auto getLogicalDevice() const -> vk::Device {
        return _logicalDevice;
    }
    [[nodiscard]] auto getPhysicalDevice() const -> vk::PhysicalDevice {
        return _physicalDevice;
    }
    [[nodiscard]] auto getMemoryResource() const -> VmaAllocator {
        return _allocator;
    }
    [[nodiscard]] auto getPresentQueue() const -> vk::Queue {
        return _presentQueue;
    }
    [[nodiscard]] auto getGraphicsQueue() const -> vk::Queue {
        return _graphicsQueue;
    }
    [[nodiscard]] auto getPresentFamily() const -> uint32_t {
        return _presentFamily;
    }
    [[nodiscard]] auto getGraphicsFamily() const -> uint32_t {
        return _graphicsFamily;
    }
    [[nodiscard]] auto getSurface() const -> vk::SurfaceKHR {
        return _surface;
    }

private:
    void _createInstance(Display& display);
    void _createSurface(Display& display);
    void _selectPhysicalDevice();
    void _createLogicalDevice();
    void _createMemoryResource();

public:
    void WaitIdle();

    auto CreateGPUFence() -> void*;
    void DestroyGPUFence(void* fence);
    void WaitOnGPUFence(void* fence);
    void ExecuteCommandBuffer(const CommandBuffer& cmd, void* fence);

    auto CreateBuffer(GraphicsBuffer::Target target, int size) -> void*;
    void DestroyBuffer(void* buffer);
    void UpdateBuffer(void* buffer, std::span<const std::byte> bytes, size_t offset);

    auto CreateCommandPool() -> void*;
    void DestroyCommandPool(void* pool);

    auto AllocateCommandBuffer(void* pool) -> void*;
    void FreeCommandBuffer(void* pool, void* cmd);

    auto CreateTexture(glm::u32 width, glm::u32 height, vk::Format format) -> void*;
    auto CreateTexture(VkImage image, VkImageView imageView, VkSampler sampler, VmaAllocation allocation) -> void*;
    void DestroyTexture(void* texture);

private:
    vk::DynamicLoader dl;

    vk::Instance _instance;
    vk::Device _logicalDevice;
    vk::PhysicalDevice _physicalDevice;

    VmaAllocator _allocator;

    vk::SurfaceKHR _surface;
    vk::DebugUtilsMessengerEXT _debugUtils;

    uint32_t _graphicsFamily;
    uint32_t _presentFamily;

    vk::Queue _presentQueue;
    vk::Queue _graphicsQueue;
};