#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

struct VulkanTexture {
    vk::Image image{};
    vk::Sampler sampler{};
    vk::ImageView imageView{};
    VmaAllocation allocation{};
};