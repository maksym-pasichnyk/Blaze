#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

struct VulkanGraphicsBuffer {
    vk::Buffer buffer{};
    VmaAllocation allocation{};
    VmaAllocationInfo allocationInfo{};
};