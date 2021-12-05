#include "CommandBuffer.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

void CommandBuffer::drawMesh(const Mesh &mesh, const Material &material) {
    _cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, material.getPipeline());
    _cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        material.getPipelineLayout(),
        0,
        material.getDescriptorSets(),
        material.getDynamicOffsets()
    );
    _cmd.bindVertexBuffers(0, *mesh.vertexBuffer, vk::DeviceSize{0});
    _cmd.bindIndexBuffer(*mesh.indexBuffer, 0, vk::IndexType::eUint32);
    _cmd.drawIndexed(mesh.indexCount, 1, 0, 0, 0);
}
