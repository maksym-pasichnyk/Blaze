#pragma once

#include <vulkan/vulkan.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_beta.h>
#include <vulkan/vk_mem_alloc.hpp>
#include <util/internal_ptr.hpp>

struct Display;
struct BlazeInstance {
    friend struct Blaze;

    BlazeInstance() = default;

public:
    auto getLogicalDevice() const -> vk::Device;
    auto getPhysicalDevice() const -> vk::PhysicalDevice;
    auto getMemoryResource() const -> VmaAllocator;
    auto getPresentQueue() const -> vk::Queue;
    auto getGraphicsQueue() const -> vk::Queue;
    auto getPresentFamily() const -> uint32_t;
    auto getGraphicsFamily() const -> uint32_t;
    auto getSurface() const -> vk::SurfaceKHR;

private:
    void init(Display& display);

private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};