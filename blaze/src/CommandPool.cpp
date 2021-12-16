#include "CommandPool.hpp"
#include "CommandBuffer.hpp"

#include <VulkanGfxDevice.hpp>

extern auto GetGfxDevice() -> VulkanGfxDevice&;

void CommandPool::Dispose::operator()(void* pool) {
    GetGfxDevice().DestroyCommandPool(pool);
}

auto CommandPool::allocate() -> CommandBuffer {
    CommandBuffer cmd;
    cmd.impl = GetGfxDevice().AllocateCommandBuffer(impl.get());
    return cmd;
}

void CommandPool::free(const CommandBuffer& cmd) {
    GetGfxDevice().FreeCommandBuffer(impl.get(), cmd.impl);
}
