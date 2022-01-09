#pragma once

#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

struct VulkanMaterial {
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;
    std::vector<uint32_t> dynamicOffsets;

    vk::DescriptorPool descriptorPool;
    vk::DescriptorSetLayout descriptorSetLayout;
    std::vector<vk::DescriptorSet> descriptorSets;

    std::vector<vk::Sampler> samplers;
};