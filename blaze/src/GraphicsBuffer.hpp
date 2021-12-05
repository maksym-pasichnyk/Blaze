#pragma once

#include <vulkan/vulkan.hpp>
#include <util/internal_ptr.hpp>

struct GraphicsBuffer {
    enum class Target {
        Vertex  = 1 << 0,
        Index   = 1 << 1,
        CopySrc = 1 << 2,
        CopyDst = 1 << 3
    };

    GraphicsBuffer() = default;
    GraphicsBuffer(Target target, vk::DeviceSize size);

    auto map() -> std::byte*;
    void unmap();
    auto size() -> vk::DeviceSize;

    void setData(std::span<const std::byte> bytes, int offset);

    auto operator *() const -> vk::Buffer;

private:
    struct Impl;

    Target _target;
    vk::DeviceSize _size;
    blaze::unique_internal_ptr<Impl> impl;
};