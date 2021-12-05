#pragma once

#include <CommandBuffer.hpp>
#include <util/internal_ptr.hpp>

struct CommandPool {
    CommandPool() = default;
    CommandPool(uint32_t queueFamilyIndex);

    auto allocate(vk::CommandBufferLevel level) -> CommandBuffer;
    void free(CommandBuffer buffer);

private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};