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
    GetGfxDevice().SetConstantBuffer(impl.get(), index, buffer);
}

void Material::SetTexture(uint32_t index, const Texture2D &texture) {
    GetGfxDevice().SetTexture(impl.get(), index, texture);
}
