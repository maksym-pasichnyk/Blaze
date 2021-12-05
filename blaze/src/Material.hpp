#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "DescriptorPool.hpp"

#include <util/internal_ptr.hpp>

struct Texture2d;
struct Material {
    static auto LoadFromResources(const std::string& filename) -> Material;

    auto getPipeline() const -> vk::Pipeline;
    auto getPipelineLayout() const -> vk::PipelineLayout;
    auto getDynamicOffsets() const -> std::span<const uint32_t>;
    auto getDescriptorSets() const -> std::span<const vk::DescriptorSet>;
    void setTexture(uint32_t index, const Texture2d& texture, vk::Sampler sampler);
private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};