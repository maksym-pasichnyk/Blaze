#include "GraphicsBuffer.hpp"
#include "Blaze.hpp"

static constexpr auto GetBufferUsageFromTarget(GraphicsBuffer::Target target) -> vk::BufferUsageFlags {
    using Type = std::underlying_type_t<GraphicsBuffer::Target>;

    auto flags = vk::BufferUsageFlags{};
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::Vertex)) {
        flags |= vk::BufferUsageFlagBits::eVertexBuffer;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::Index)) {
        flags |= vk::BufferUsageFlagBits::eIndexBuffer;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopySrc)) {
        flags |= vk::BufferUsageFlagBits::eTransferSrc;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopyDst)) {
        flags |= vk::BufferUsageFlagBits::eTransferDst;
    }
    return flags;
}

static constexpr auto GetMemoryUsageFromTarget(GraphicsBuffer::Target target) -> VmaMemoryUsage {
    using Type = std::underlying_type_t<GraphicsBuffer::Target>;

    auto flags = VMA_MEMORY_USAGE_CPU_TO_GPU;
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopySrc)) {
        flags = VMA_MEMORY_USAGE_CPU_ONLY;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopyDst)) {
        flags = VMA_MEMORY_USAGE_GPU_ONLY;
    }
    return flags;
}

struct GraphicsBuffer::Impl {
    VkBuffer _buffer;
    VmaAllocation _allocation;
    VmaAllocationInfo _allocationInfo;

    Impl(Target target, vk::DeviceSize size) {
        const auto bufferCreateInfo = static_cast<VkBufferCreateInfo>(vk::BufferCreateInfo {
            .size = size,
            .usage = GetBufferUsageFromTarget(target)
        });

        const auto allocCreateInfo = VmaAllocationCreateInfo {
            .usage = GetMemoryUsageFromTarget(target)
        };

        vmaCreateBuffer(
            Blaze::GetMemoryResource(),
            &bufferCreateInfo,
            &allocCreateInfo,
            &_buffer,
            &_allocation,
            &_allocationInfo
        );
    }

    ~Impl() {
        vmaDestroyBuffer(Blaze::GetMemoryResource(), _buffer, _allocation);
    }
};

GraphicsBuffer::GraphicsBuffer(Target target, vk::DeviceSize size) : _target(target), _size(size) {
    impl = blaze::make_internal<Impl>(target, size);
}

auto GraphicsBuffer::map() -> std::byte* {
    void* p;
    vmaMapMemory(Blaze::GetMemoryResource(), impl->_allocation, &p);
    return static_cast<std::byte*>(p);
}

void GraphicsBuffer::unmap() {
    vmaUnmapMemory(Blaze::GetMemoryResource(), impl->_allocation);
}

auto GraphicsBuffer::size() -> vk::DeviceSize {
    return _size;
}

void GraphicsBuffer::setData(std::span<const std::byte> bytes, int offset) {
    auto dst = std::span(map(), _size).subspan(offset);
    std::copy(bytes.begin(),  bytes.end(), dst.begin());
    unmap();
}

auto GraphicsBuffer::operator*() const -> vk::Buffer {
    return impl->_buffer;
}