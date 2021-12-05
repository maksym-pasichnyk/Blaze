#pragma once

#include <span>
#include <vulkan/vulkan.hpp>
#include <util/internal_ptr.hpp>

struct DescriptorPool {
    DescriptorPool() = default;
    DescriptorPool(uint32_t maxSets, std::span<const vk::DescriptorPoolSize> poolSizes);

    auto allocate(vk::DescriptorSetLayout setLayout) -> vk::DescriptorSet;
    void free(vk::DescriptorSet descriptorSet);

private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};