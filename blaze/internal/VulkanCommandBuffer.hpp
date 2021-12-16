#pragma once

#include <vulkan/vulkan_core.h>

struct VulkanCommandBuffer {
    vk::CommandPool pool{};
    vk::CommandBuffer cmd{};
};