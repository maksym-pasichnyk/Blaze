#include "GraphicsFence.hpp"

#include <VulkanGfxDevice.hpp>

extern auto GetGfxDevice() -> VulkanGfxDevice&;

void GraphicsFence::Dispose::operator()(void* fence) {
    GetGfxDevice().DestroyGPUFence(fence);
}
