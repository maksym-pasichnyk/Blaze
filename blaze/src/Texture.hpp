#pragma once

#include <span>
#include <memory>
#include <glm/glm.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "TextureData.hpp"

enum class GraphicsFormat {
};

struct Texture {
    friend struct Texture2D;

    Texture() = default;
    Texture(VkImage image, VkImageView view, VkSampler sampler, VmaAllocation allocation);

    auto getImage() const -> vk::Image;
    auto getSampler() const -> vk::Sampler;
    auto getImageView() const -> vk::ImageView;

private:
    struct Dispose {
        void operator()(void*);
    };
    std::unique_ptr<void, Dispose> impl;
};

struct Texture2D : protected Texture {
    using Texture::getImage;
    using Texture::getSampler;
    using Texture::getImageView;

    Texture2D() = default;
    Texture2D(glm::u32 width, glm::u32 height, vk::Format format);

    void setPixels(std::span<const glm::u8vec4> pixels);

    auto width() const -> glm::u32 {
        return _width;
    }
    auto height() const -> glm::u32 {
        return _height;
    }

private:
    glm::u32 _width{};
    glm::u32 _height{};
};

struct RenderTextureDescriptor {
    glm::u32 width;
    glm::u32 height;
    vk::Format depthStencilFormat;
};

struct RenderTexture {
    RenderTexture() = default;
    explicit RenderTexture(const RenderTextureDescriptor& descriptor);

private:
    struct Dispose {
        void operator()(void*);
    };
    RenderTextureDescriptor descriptor;
    std::unique_ptr<void, Dispose> impl;
};

//struct RenderBuffer {
//
//private:
//    Texture _texture;
//};