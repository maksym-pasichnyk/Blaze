#pragma once

#include <string>
#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>

struct Texture2D;
struct GraphicsBuffer;

struct Material {
    static auto LoadFromResources(const std::string& filename) -> Material;

    void SetConstantBuffer(uint32_t index, const GraphicsBuffer& buffer);
    void SetTexture(uint32_t index, const Texture2D& texture);
    auto GetNativeHandlePtr() const -> void* {
        return impl.get();
    }
private:
    struct Dispose {
        void operator()(void*);
    };
    std::unique_ptr<void, Dispose> impl;
};