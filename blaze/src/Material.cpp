#include "Blaze.hpp"
#include "VulkanGfxDevice.hpp"
#include "VulkanMaterial.hpp"
#include <Texture.hpp>
#include <Material.hpp>
#include <Resources.hpp>
#include <GraphicsBuffer.hpp>
#include <VulkanGraphicsBuffer.hpp>

extern auto GetGfxDevice() -> VulkanGfxDevice&;

void Material::Dispose::operator()(void* material) {
    GetGfxDevice().DestroyMaterial(material);
}

auto Material::LoadFromResources(const std::string& filename) -> Material {
    Material material{};
    material.impl.reset(GetGfxDevice().CreateMaterial(Resources::get(filename).value()));
    return material;
}

void Material::SetConstantBuffer(uint32_t index, const GraphicsBuffer& buffer) {
    auto vk_material = static_cast<VulkanMaterial*>(impl.get());
    auto vk_buffer = static_cast<VulkanGraphicsBuffer*>(buffer.getNativeBufferPtr());

    const auto bufferInfo = vk::DescriptorBufferInfo {
        .buffer = vk_buffer->buffer,
        .offset = 0,
        .range = static_cast<vk::DeviceSize>(buffer.getSize())
    };
    const auto writeDescriptorSet = vk::WriteDescriptorSet{
        .dstSet = vk_material->descriptorSets[0],
        .dstBinding = index,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo = &bufferInfo
    };

    GetGfxDevice().getLogicalDevice().updateDescriptorSets({writeDescriptorSet}, {});
}

void Material::SetTexture(uint32_t index, const Texture2D &texture) {
    auto vk_material = static_cast<VulkanMaterial*>(impl.get());
    const auto imageInfo = vk::DescriptorImageInfo{
        .sampler = texture.getSampler(),
        .imageView = texture.getImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
    };

    const auto writeDescriptorSet = vk::WriteDescriptorSet{
        .dstSet = vk_material->descriptorSets[0],
        .dstBinding = index,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo = &imageInfo
    };

    GetGfxDevice().getLogicalDevice().updateDescriptorSets({writeDescriptorSet}, {});
}
