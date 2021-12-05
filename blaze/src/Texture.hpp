#pragma once

#include <span>
#include <memory>
#include <glm/glm.hpp>
#include <util/internal_ptr.hpp>

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_mem_alloc.hpp>

#include "TextureData.hpp"

struct Texture {
    friend struct Texture2d;

    Texture() = default;
    Texture(VkImage image, VkImageView view, VkSampler sampler, VmaAllocation allocation);

    auto getSampler() const -> vk::Sampler;
    auto getImageView() const -> vk::ImageView;

    static auto CreateDepthTexture(vk::Format format, const vk::Extent2D& extent) -> Texture;

private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};

struct Texture2d {
    Texture2d() = default;
    Texture2d(const TextureExtent& extent/*, vk::Format format*/);
    Texture2d(glm::u32 width, glm::u32 height/*, vk::Format format*/)
        : Texture2d(TextureExtent{width, height}/*, format*/) {}

    void setPixels(std::span<const glm::u8vec4> pixels);

    auto getSampler() const -> vk::Sampler {
        return _texture.getSampler();
    }

    auto getImageView() const -> vk::ImageView {
        return _texture.getImageView();
    }
private:
    TextureExtent _extent{};
    Texture _texture;
};

struct RenderBuffer {

private:
    Texture _texture;
};