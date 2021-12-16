#include "Graphics.hpp"
#include "CommandPool.hpp"
#include "GraphicsFence.hpp"
#include "GraphicsBuffer.hpp"

#include <VulkanGfxDevice.hpp>

extern auto GetGfxDevice() -> VulkanGfxDevice&;

auto Graphics::CreateGraphicsFence() -> GraphicsFence {
    GraphicsFence fence;
    fence.impl.reset(GetGfxDevice().CreateGPUFence());
    return fence;
}

void Graphics::WaitOnGraphicsFence(const GraphicsFence& fence) {
    GetGfxDevice().WaitOnGPUFence(fence.impl.get());
}

void Graphics::ExecuteCommandBuffer(const CommandBuffer& cmd, const GraphicsFence& fence) {
    return GetGfxDevice().ExecuteCommandBuffer(cmd, fence.impl.get());
}

auto Graphics::CreateCommandPool() -> CommandPool {
    CommandPool pool;
    pool.impl.reset(GetGfxDevice().CreateCommandPool());
    return pool;
}
