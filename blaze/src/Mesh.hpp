#pragma once

#include "GraphicsBuffer.hpp"

struct Mesh {
    GraphicsBuffer vertexBuffer{};
    GraphicsBuffer indexBuffer{};
    int vertexCount = 0;
    int indexCount = 0;

    void setVertexBufferParams(int count, size_t elementSize) {
        const auto bufferSize = static_cast<vk::DeviceSize>(count) * elementSize;

        vertexCount = count;
        if (bufferSize > vertexBuffer.size()) {
            vertexBuffer = GraphicsBuffer(GraphicsBuffer::Target::Vertex, bufferSize);
        }
    }

    void setVertexBufferData(std::span<const std::byte> bytes, int offset) {
        vertexBuffer.setData(bytes, offset);
    }

    void setIndexBufferParams(int count, size_t elementSize) {
        const auto bufferSize = static_cast<vk::DeviceSize>(count) * elementSize;

        indexCount = count;
        if (bufferSize > indexBuffer.size()) {
            indexBuffer = GraphicsBuffer(GraphicsBuffer::Target::Index, bufferSize);
        }
    }
    void setIndexBufferData(std::span<const std::byte> bytes, int offset) {
        indexBuffer.setData(bytes, offset);
    }
};
