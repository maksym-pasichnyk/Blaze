#pragma once

#include <span>
#include <memory>
#include <glm/glm.hpp>

struct TextureExtent {
    glm::u32 width;
    glm::u32 height;

    template <typename T>
    [[nodiscard]] auto into() const -> T;
};

struct TextureData {
    TextureData() = default;
    TextureData(glm::u32 width, glm::u32 height)
        : _width(width)
        , _height(height)
        , _pixels(std::make_unique<glm::u8vec4[]>(static_cast<size_t>(width) * height)) {}

    [[nodiscard]] auto getExtent() const -> TextureExtent {
        return {_width, _height};
    }

    [[nodiscard]] auto getPixels() const & -> std::span<const glm::u8vec4> {
        return {_pixels.get(), static_cast<size_t>(_width) * _height};
    }

    [[nodiscard]] auto getPixel(glm::u32 x, glm::u32 y) const -> glm::u8vec4 {
        assert(x <= _width && y <= _height);
        return _pixels[static_cast<size_t>(y) * _width + x];
    }

    void setPixel(glm::u32 x, glm::u32 y, const glm::u8vec4& pixel) & {
        assert(x <= _width && y <= _height);
        _pixels[static_cast<size_t>(y) * _width + x] = pixel;
    }

    void setPixel(glm::u32 x, glm::u32 y, const glm::vec4& pixel) & {
        assert(x <= _width && y <= _height);
        const auto p = glm::clamp(pixel, 0.0f, 1.0f);
        _pixels[static_cast<size_t>(y) * _width + x] = glm::u8vec4(p * 255.0f);
    }

private:
    glm::u32 _width = 0;
    glm::u32 _height = 0;
    std::unique_ptr<glm::u8vec4[]> _pixels;
};
