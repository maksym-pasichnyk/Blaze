#pragma once

#include "GraphicsBuffer.hpp"
#include <glm/fwd.hpp>
#include <vector>

struct SubMesh {
    glm::i32 indexCount;
    glm::i32 indexOffset;
};

struct Mesh {
    void setVertexBufferParams(int count, size_t elementSize) {
        const auto bufferSize = static_cast<int>(count * elementSize);

        _vertexCount = count;
        if (bufferSize > _vertexBuffer.getSize()) {
            _vertexBuffer = GraphicsBuffer(GraphicsBuffer::Target::Vertex, bufferSize);
        }
    }

    void setVertexBufferData(std::span<const std::byte> bytes, int offset) {
        _vertexBuffer.setData(bytes, offset);
    }

    void setVertexBufferData(const void* bytes, int len, int offset) {
        _vertexBuffer.setData(bytes, len, offset);
    }

    void setIndexBufferParams(int count, size_t elementSize) {
        const auto bufferSize = static_cast<int>(count * elementSize);

        _indexCount = count;
        if (bufferSize > _indexBuffer.getSize()) {
            _indexBuffer = GraphicsBuffer(GraphicsBuffer::Target::Index, bufferSize);
        }
    }
    void setIndexBufferData(std::span<const std::byte> bytes, int offset) {
        _indexBuffer.setData(bytes, offset);
    }

    void setIndexBufferData(const void* bytes, int len, int offset) {
        _indexBuffer.setData(bytes, len, offset);
    }

    [[nodiscard]] auto getNativeVertexBufferPtr() const -> void* {
        return _vertexBuffer.getNativeBufferPtr();
    }

    [[nodiscard]] auto getNativeIndexBufferPtr() const -> void* {
        return _indexBuffer.getNativeBufferPtr();
    }

    [[nodiscard]] auto getVertexCount() const -> int {
        return _vertexCount;
    }

    [[nodiscard]] auto getIndexCount() const -> int {
        return _indexCount;
    }

    void setSubmeshCount(int count) {
        _submeshes.clear();
        _submeshes.resize(count);
    }

    void setSubmesh(int index, const SubMesh& submesh) {
        _submeshes.at(index) = submesh;
    }

    [[nodiscard]] auto getSubmesh(int index) const -> SubMesh {
        return _submeshes.at(index);
    }

private:
    GraphicsBuffer _vertexBuffer{};
    GraphicsBuffer _indexBuffer{};

    int _vertexCount = 0;
    int _indexCount = 0;

    std::vector<SubMesh> _submeshes;
};
