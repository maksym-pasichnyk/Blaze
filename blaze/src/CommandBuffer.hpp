#pragma once

#include <vulkan/vulkan.hpp>

struct Mesh;
struct Material;
struct CommandBuffer {
    void drawMesh(const Mesh& mesh, const Material& material);
    
    auto operator*() const -> vk::CommandBuffer {
        return _cmd;
    }

private:
    vk::CommandBuffer _cmd;
};