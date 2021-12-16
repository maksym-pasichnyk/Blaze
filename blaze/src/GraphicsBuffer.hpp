#pragma once

#include <span>
#include <memory>

struct GraphicsBuffer {
    friend struct Graphics;

    enum class Target {
        Vertex   = 1 << 0,
        Index    = 1 << 1,
        CopySrc  = 1 << 2,
        CopyDst  = 1 << 3,
        Constant = 1 << 4
    };

    GraphicsBuffer() = default;
    GraphicsBuffer(Target target, int size);

    void setData(std::span<const std::byte> bytes, int offset);
    void setData(const void* ptr, int len, int offset) {
        setData(std::span(static_cast<const std::byte *>(ptr), len), offset);
    }

    [[nodiscard]] auto getNativeBufferPtr() const -> void* {
        return impl.get();
    }
    [[nodiscard]] auto getSize() const -> int {
        return size;
    }

private:
    struct Dispose {
        void operator()(void*);
    };

    int size{};
    Target target{};
    std::unique_ptr<void, Dispose> impl;
};