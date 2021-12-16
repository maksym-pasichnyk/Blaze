#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <util/internal_ptr.hpp>

struct Texture2D;
struct GraphicsBuffer;

struct Material {
    static auto LoadFromResources(const std::string& filename) -> Material;

    auto getPipeline() const -> vk::Pipeline;
    auto getPipelineLayout() const -> vk::PipelineLayout;
    auto getDynamicOffsets() const -> std::span<const uint32_t>;
    auto getDescriptorSets() const -> std::span<const vk::DescriptorSet>;

    void SetConstantBuffer(uint32_t index, const GraphicsBuffer& buffer);
    void SetTexture(uint32_t index, const Texture2D& texture);
private:
    struct Impl;
    blaze::shared_internal_ptr<Impl> impl;
};