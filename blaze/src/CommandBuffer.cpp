#include "CommandBuffer.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

#include <VulkanGraphicsBuffer.hpp>

void CommandBuffer::drawMesh(const Mesh &mesh, const Material &material) {
    auto _cmd = **this;

    auto vk_vertexBuffer = static_cast<VulkanGraphicsBuffer*>(mesh.getNativeVertexBufferPtr());
    auto vk_indexBuffer = static_cast<VulkanGraphicsBuffer*>(mesh.getNativeIndexBufferPtr());

    _cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material.getPipeline());
    _cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        material.getPipelineLayout(),
        0,
        material.getDescriptorSets(),
        material.getDynamicOffsets()
    );
    _cmd.bindVertexBuffers(0, vk_vertexBuffer->buffer, vk::DeviceSize{0});
    _cmd.bindIndexBuffer(vk_indexBuffer->buffer, 0, vk::IndexType::eUint32);
    _cmd.drawIndexed(mesh.getIndexCount(), 1, 0, 0, 0);
}

void CommandBuffer::clearRenderTarget(const glm::vec4& color, float depth, float stencil) {
//    const auto clearAttachments = std::array{
//        vk::ClearAttachment{
//            .aspectMask = vk::ImageAspectFlagBits::eColor,
//            .colorAttachment = 0,
//            .clearValue = {
//                .color = {
//                    .float32 = std::array{color.x, color.y, color.z, color.w}
//                }
//            }
//        },
//        vk::ClearAttachment{
//            .aspectMask = vk::ImageAspectFlagBits::eDepth,
//            .clearValue = {
//                .depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 }
//            }
//        }
//    };
//    const auto clearRect = vk::ClearRect{
//        .rect = {
//            .offset = vk::Offset2D{ 0, 0 },
//            .extent = extent
//        },
//        .layerCount = 1
//    };
//    _cmd.clearAttachments(clearAttachments, {clearRect});
}