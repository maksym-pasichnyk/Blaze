#include "GraphicsBuffer.hpp"

#include <VulkanGfxDevice.hpp>

extern auto GetGfxDevice() -> VulkanGfxDevice&;

void GraphicsBuffer::Dispose::operator()(void *buffer) {
    GetGfxDevice().DestroyBuffer(buffer);
}

GraphicsBuffer::GraphicsBuffer(Target target, int size) : size(size), target(target) {
    impl.reset(GetGfxDevice().CreateBuffer(target, size));
}

void GraphicsBuffer::setData(std::span<const std::byte> bytes, int offset) {
    GetGfxDevice().UpdateBuffer(impl.get(), bytes, offset);
}