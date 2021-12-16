#pragma once

#include <glm/vec4.hpp>
#include <vulkan/vulkan.hpp>

struct Mesh;
struct Material;
struct CommandBuffer {
    friend struct CommandPool;

    void drawMesh(const Mesh& mesh, const Material& material);
    void drawMesh(const Mesh& mesh, const Material& material, int submeshIndex);
    void clearRenderTarget(const glm::vec4& color, float depth, float stencil);
    
    auto operator*() const -> vk::CommandBuffer {
        return static_cast<VkCommandBuffer>(impl);
    }

private:
    void* impl = nullptr;
};