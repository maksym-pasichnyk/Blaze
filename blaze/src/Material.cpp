#include "Json.hpp"
#include "Blaze.hpp"
#include <Texture.hpp>
#include <Material.hpp>
#include <Resources.hpp>
#include <GraphicsBuffer.hpp>
#include <VulkanGraphicsBuffer.hpp>

#include <spirv_glsl.hpp>

template <>
struct Json::Into<vk::ShaderStageFlagBits> {
    static auto into(const Json& o) -> std::optional<vk::ShaderStageFlagBits> {
        auto&& s = o.to_string();
        if (s == "vertex") {
            return vk::ShaderStageFlagBits::eVertex;
        }
        if (s == "fragment") {
            return vk::ShaderStageFlagBits::eFragment;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::ShaderStageFlags> {
    static auto into(const Json& o) -> std::optional<vk::ShaderStageFlags> {
        auto&& s = o.to_string();
        if (s == "vertex") {
            return vk::ShaderStageFlagBits::eVertex;
        }
        if (s == "fragment") {
            return vk::ShaderStageFlagBits::eFragment;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::CullModeFlags> {
    static auto into(const Json& o) -> std::optional<vk::CullModeFlags> {
        static const auto table = std::map<std::string, vk::CullModeFlags>{
            { "front",             vk::CullModeFlagBits::eFront },
            { "back",              vk::CullModeFlagBits::eBack },
            { "front_and_back",    vk::CullModeFlagBits::eFrontAndBack },
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::DescriptorType> {
    static auto into(const Json& o) -> std::optional<vk::DescriptorType> {
        static const auto table = std::map<std::string, vk::DescriptorType>{
            { "sampler",                    vk::DescriptorType::eSampler },
            { "combined_image_sampler",     vk::DescriptorType::eCombinedImageSampler },
            { "sampled_image",              vk::DescriptorType::eSampledImage },
            { "storage_image",              vk::DescriptorType::eStorageImage },
            { "uniform_texel_buffer",       vk::DescriptorType::eUniformTexelBuffer },
            { "storage_texel_buffer",       vk::DescriptorType::eStorageTexelBuffer },
            { "uniform_buffer",             vk::DescriptorType::eUniformBuffer },
            { "storage_buffer",             vk::DescriptorType::eStorageBuffer },
            { "uniform_buffer_dynamic",     vk::DescriptorType::eUniformBufferDynamic },
            { "storage_buffer_dynamic",     vk::DescriptorType::eStorageBufferDynamic },
            { "input_attachment",           vk::DescriptorType::eInputAttachment },
//            { "inline_uniform_block_ext",   vk::DescriptorType::eInlineUniformBlockEXT },
//            { "acceleration_structure_khr", vk::DescriptorType::eAccelerationStructureKHR },
//            { "acceleration_structure_nv",  vk::DescriptorType::eAccelerationStructureNV },
//            { "mutable_valve",              vk::DescriptorType::eMutableVALVE }
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::BlendFactor> {
    static auto into(const Json& o) -> std::optional<vk::BlendFactor> {
        static const auto table = std::map<std::string, vk::BlendFactor>{
            {"zero",                     vk::BlendFactor::eZero },
            {"one",                      vk::BlendFactor::eOne },
            {"src_color",                vk::BlendFactor::eSrcColor },
            {"one_minus_src_color",      vk::BlendFactor::eOneMinusSrcColor },
            {"dst_color",                vk::BlendFactor::eDstColor },
            {"one_minus_dst_color",      vk::BlendFactor::eOneMinusDstColor },
            {"src_alpha",                vk::BlendFactor::eSrcAlpha },
            {"one_minus_src_alpha",      vk::BlendFactor::eOneMinusSrcAlpha },
            {"dst_alpha",                vk::BlendFactor::eDstAlpha },
            {"one_minus_dst_alpha",      vk::BlendFactor::eOneMinusDstAlpha },
            {"constant_color",           vk::BlendFactor::eConstantColor },
            {"one_minus_constant_color", vk::BlendFactor::eOneMinusConstantColor },
            {"constant_alpha",           vk::BlendFactor::eConstantAlpha },
            {"one_minus_constant_alpha", vk::BlendFactor::eOneMinusConstantAlpha },
            {"src_alpha_saturate",       vk::BlendFactor::eSrcAlphaSaturate },
            {"src1_color",               vk::BlendFactor::eSrc1Color },
            {"one_minus_src1_color",     vk::BlendFactor::eOneMinusSrc1Color },
            {"src1_alpha",               vk::BlendFactor::eSrc1Alpha },
            {"one_minus_src1_alpha",     vk::BlendFactor::eOneMinusSrc1Alpha }
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::PrimitiveTopology> {
    static auto into(const Json& o) -> std::optional<vk::PrimitiveTopology> {
        static const auto table = std::map<std::string, vk::PrimitiveTopology>{
            {"point_list",                    vk::PrimitiveTopology::ePointList},
            {"line_list",                     vk::PrimitiveTopology::eLineList},
            {"line_strip",                    vk::PrimitiveTopology::eLineStrip},
            {"triangle_list",                 vk::PrimitiveTopology::eTriangleList},
            {"triangle_strip",                vk::PrimitiveTopology::eTriangleStrip},
            {"triangle_fan",                  vk::PrimitiveTopology::eTriangleFan},
            {"line_list_with_adjacency",      vk::PrimitiveTopology::eLineListWithAdjacency},
            {"line_strip_with_adjacency",     vk::PrimitiveTopology::eLineStripWithAdjacency},
            {"triangle_list_with_adjacency",  vk::PrimitiveTopology::eTriangleListWithAdjacency},
            {"triangle_strip_with_adjacency", vk::PrimitiveTopology::eTriangleStripWithAdjacency},
            {"patch_list",                    vk::PrimitiveTopology::ePatchList}
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::FrontFace> {
    static auto into(const Json& o) -> std::optional<vk::FrontFace> {
        static const auto table = std::map<std::string, vk::FrontFace>{
            {"clockwise",         vk::FrontFace::eClockwise},
            {"counter_clockwise", vk::FrontFace::eCounterClockwise}
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::PolygonMode> {
    static auto into(const Json& o) -> std::optional<vk::PolygonMode> {
        static const auto table = std::map<std::string, vk::PolygonMode>{
            { "fill",              vk::PolygonMode::eFill },
            { "line",              vk::PolygonMode::eLine },
            { "point",             vk::PolygonMode::ePoint },
            { "fill_rectangle_nv", vk::PolygonMode::eFillRectangleNV },
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::CompareOp> {
    static auto into(const Json& o) -> std::optional<vk::CompareOp> {
        static const auto table = std::map<std::string, vk::CompareOp>{
            { "never",            vk::CompareOp::eNever },
            { "less",             vk::CompareOp::eLess },
            { "equal",            vk::CompareOp::eEqual },
            { "less_or_equal",    vk::CompareOp::eLessOrEqual },
            { "greater",          vk::CompareOp::eGreater },
            { "not_equal",        vk::CompareOp::eNotEqual },
            { "greater_or_equal", vk::CompareOp::eGreaterOrEqual },
            { "always",           vk::CompareOp::eAlways }
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::BlendOp> {
    static auto into(const Json& o) -> std::optional<vk::BlendOp> {
        static const auto table = std::map<std::string, vk::BlendOp>{
            { "add",                    vk::BlendOp::eAdd },
            { "subtract",               vk::BlendOp::eSubtract },
            { "reverse_subtract",       vk::BlendOp::eReverseSubtract },
            { "min",                    vk::BlendOp::eMin },
            { "max",                    vk::BlendOp::eMax },
            { "zero_ext",               vk::BlendOp::eZeroEXT },
            { "src_ext",                vk::BlendOp::eSrcEXT },
            { "dst_ext",                vk::BlendOp::eDstEXT },
            { "src_over_ext",           vk::BlendOp::eSrcOverEXT },
            { "dst_over_ext",           vk::BlendOp::eDstOverEXT },
            { "src_in_ext",             vk::BlendOp::eSrcInEXT },
            { "dst_in_ext",             vk::BlendOp::eDstInEXT },
            { "src_out_ext",            vk::BlendOp::eSrcOutEXT },
            { "dst_out_ext",            vk::BlendOp::eDstOutEXT },
            { "src_atop_ext",           vk::BlendOp::eSrcAtopEXT },
            { "dst_atop_ext",           vk::BlendOp::eDstAtopEXT },
            { "xor_ext",                vk::BlendOp::eXorEXT },
            { "multiply_ext",           vk::BlendOp::eMultiplyEXT },
            { "screen_ext",             vk::BlendOp::eScreenEXT },
            { "overlay_ext",            vk::BlendOp::eOverlayEXT },
            { "darken_ext",             vk::BlendOp::eDarkenEXT },
            { "lighten_ext",            vk::BlendOp::eLightenEXT },
            { "colordodge_ext",         vk::BlendOp::eColordodgeEXT },
            { "colorburn_ext",          vk::BlendOp::eColorburnEXT },
            { "hardlight_ext",          vk::BlendOp::eHardlightEXT },
            { "softlight_ext",          vk::BlendOp::eSoftlightEXT },
            { "difference_ext",         vk::BlendOp::eDifferenceEXT },
            { "exclusion_ext",          vk::BlendOp::eExclusionEXT },
            { "invert_ext",             vk::BlendOp::eInvertEXT },
            { "invert_rgb_ext",         vk::BlendOp::eInvertRgbEXT },
            { "lineardodge_ext",        vk::BlendOp::eLineardodgeEXT },
            { "linearburn_ext",         vk::BlendOp::eLinearburnEXT },
            { "vividlight_ext",         vk::BlendOp::eVividlightEXT },
            { "linearlight_ext",        vk::BlendOp::eLinearlightEXT },
            { "pinlight_ext",           vk::BlendOp::ePinlightEXT },
            { "hardmix_ext",            vk::BlendOp::eHardmixEXT },
            { "hsl_hue_ext",            vk::BlendOp::eHslHueEXT },
            { "hsl_saturation_ext",     vk::BlendOp::eHslSaturationEXT },
            { "hsl_color_ext",          vk::BlendOp::eHslColorEXT },
            { "hsl_luminosity_ext",     vk::BlendOp::eHslLuminosityEXT },
            { "plus_ext",               vk::BlendOp::ePlusEXT },
            { "plus_clamped_ext",       vk::BlendOp::ePlusClampedEXT },
            { "plus_clamped_alpha_ext", vk::BlendOp::ePlusClampedAlphaEXT },
            { "plus_darker_ext",        vk::BlendOp::ePlusDarkerEXT },
            { "minus_ext",              vk::BlendOp::eMinusEXT },
            { "minus_clamped_ext",      vk::BlendOp::eMinusClampedEXT },
            { "contrast_ext",           vk::BlendOp::eContrastEXT },
            { "invert_ovg_ext",         vk::BlendOp::eInvertOvgEXT },
            { "red_ext",                vk::BlendOp::eRedEXT },
            { "green_ext",              vk::BlendOp::eGreenEXT },
            { "blue_ext",               vk::BlendOp::eBlueEXT }
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::Format> {
    static auto into(const Json& o) -> std::optional<vk::Format> {
        static const auto table = std::map<std::string, vk::Format>{
            { "undefined", vk::Format::eUndefined },
            { "r4g4_unorm_pack8", vk::Format::eR4G4UnormPack8 },
            { "r4g4b4a4_unorm_pack16", vk::Format::eR4G4B4A4UnormPack16 },
            { "b4g4r4a4_unorm_pack16", vk::Format::eB4G4R4A4UnormPack16 },
            { "r5g6b5_unorm_pack16", vk::Format::eR5G6B5UnormPack16 },
            { "b5g6r5_unorm_pack16", vk::Format::eB5G6R5UnormPack16 },
            { "r5g5b5a1_unorm_pack16", vk::Format::eR5G5B5A1UnormPack16 },
            { "b5g5r5a1_unorm_pack16", vk::Format::eB5G5R5A1UnormPack16 },
            { "a1r5g5b5_unorm_pack16", vk::Format::eA1R5G5B5UnormPack16 },
            { "r8_unorm", vk::Format::eR8Unorm },
            { "r8_snorm", vk::Format::eR8Snorm },
            { "r8_uscaled", vk::Format::eR8Uscaled },
            { "r8_sscaled", vk::Format::eR8Sscaled },
            { "r8_uint", vk::Format::eR8Uint },
            { "r8_sint", vk::Format::eR8Sint },
            { "r8_srgb", vk::Format::eR8Srgb },
            { "r8g8_unorm", vk::Format::eR8G8Unorm },
            { "r8g8_snorm", vk::Format::eR8G8Snorm },
            { "r8g8_uscaled", vk::Format::eR8G8Uscaled },
            { "r8g8_sscaled", vk::Format::eR8G8Sscaled },
            { "r8g8_uint", vk::Format::eR8G8Uint },
            { "r8g8_sint", vk::Format::eR8G8Sint },
            { "r8g8_srgb", vk::Format::eR8G8Srgb },
            { "r8g8b8_unorm", vk::Format::eR8G8B8Unorm },
            { "r8g8b8_snorm", vk::Format::eR8G8B8Snorm },
            { "r8g8b8_uscaled", vk::Format::eR8G8B8Uscaled },
            { "r8g8b8_sscaled", vk::Format::eR8G8B8Sscaled },
            { "r8g8b8_uint", vk::Format::eR8G8B8Uint },
            { "r8g8b8_sint", vk::Format::eR8G8B8Sint },
            { "r8g8b8_srgb", vk::Format::eR8G8B8Srgb },
            { "b8g8r8_unorm", vk::Format::eB8G8R8Unorm },
            { "b8g8r8_snorm", vk::Format::eB8G8R8Snorm },
            { "b8g8r8_uscaled", vk::Format::eB8G8R8Uscaled },
            { "b8g8r8_sscaled", vk::Format::eB8G8R8Sscaled },
            { "b8g8r8_uint", vk::Format::eB8G8R8Uint },
            { "b8g8r8_sint", vk::Format::eB8G8R8Sint },
            { "b8g8r8_srgb", vk::Format::eB8G8R8Srgb },
            { "r8g8b8a8_unorm", vk::Format::eR8G8B8A8Unorm },
            { "r8g8b8a8_snorm", vk::Format::eR8G8B8A8Snorm },
            { "r8g8b8a8_uscaled", vk::Format::eR8G8B8A8Uscaled },
            { "r8g8b8a8_sscaled", vk::Format::eR8G8B8A8Sscaled },
            { "r8g8b8a8_uint", vk::Format::eR8G8B8A8Uint },
            { "r8g8b8a8_sint", vk::Format::eR8G8B8A8Sint },
            { "r8g8b8a8_srgb", vk::Format::eR8G8B8A8Srgb },
            { "b8g8r8a8_unorm", vk::Format::eB8G8R8A8Unorm },
            { "b8g8r8a8_snorm", vk::Format::eB8G8R8A8Snorm },
            { "b8g8r8a8_uscaled", vk::Format::eB8G8R8A8Uscaled },
            { "b8g8r8a8_sscaled", vk::Format::eB8G8R8A8Sscaled },
            { "b8g8r8a8_uint", vk::Format::eB8G8R8A8Uint },
            { "b8g8r8a8_sint", vk::Format::eB8G8R8A8Sint },
            { "b8g8r8a8_srgb", vk::Format::eB8G8R8A8Srgb },
            { "a8b8g8r8_unorm_pack32", vk::Format::eA8B8G8R8UnormPack32 },
            { "a8b8g8r8_snorm_pack32", vk::Format::eA8B8G8R8SnormPack32 },
            { "a8b8g8r8_uscaled_pack32", vk::Format::eA8B8G8R8UscaledPack32 },
            { "a8b8g8r8_sscaled_pack32", vk::Format::eA8B8G8R8SscaledPack32 },
            { "a8b8g8r8_uint_pack32", vk::Format::eA8B8G8R8UintPack32 },
            { "a8b8g8r8_sint_pack32", vk::Format::eA8B8G8R8SintPack32 },
            { "a8b8g8r8_srgb_pack32", vk::Format::eA8B8G8R8SrgbPack32 },
            { "a2r10g10b10_unorm_pack32", vk::Format::eA2R10G10B10UnormPack32 },
            { "a2r10g10b10_snorm_pack32", vk::Format::eA2R10G10B10SnormPack32 },
            { "a2r10g10b10_uscaled_pack32", vk::Format::eA2R10G10B10UscaledPack32 },
            { "a2r10g10b10_sscaled_pack32", vk::Format::eA2R10G10B10SscaledPack32 },
            { "a2r10g10b10_uint_pack32", vk::Format::eA2R10G10B10UintPack32 },
            { "a2r10g10b10_sint_pack32", vk::Format::eA2R10G10B10SintPack32 },
            { "a2b10g10r10_unorm_pack32", vk::Format::eA2B10G10R10UnormPack32 },
            { "a2b10g10r10_snorm_pack32", vk::Format::eA2B10G10R10SnormPack32 },
            { "a2b10g10r10_uscaled_pack32", vk::Format::eA2B10G10R10UscaledPack32 },
            { "a2b10g10r10_sscaled_pack32", vk::Format::eA2B10G10R10SscaledPack32 },
            { "a2b10g10r10_uint_pack32", vk::Format::eA2B10G10R10UintPack32 },
            { "a2b10g10r10_sint_pack32", vk::Format::eA2B10G10R10SintPack32 },
            { "r16_unorm", vk::Format::eR16Unorm },
            { "r16_snorm", vk::Format::eR16Snorm },
            { "r16_uscaled", vk::Format::eR16Uscaled },
            { "r16_sscaled", vk::Format::eR16Sscaled },
            { "r16_uint", vk::Format::eR16Uint },
            { "r16_sint", vk::Format::eR16Sint },
            { "r16_sfloat", vk::Format::eR16Sfloat },
            { "r16g16_unorm", vk::Format::eR16G16Unorm },
            { "r16g16_snorm", vk::Format::eR16G16Snorm },
            { "r16g16_uscaled", vk::Format::eR16G16Uscaled },
            { "r16g16_sscaled", vk::Format::eR16G16Sscaled },
            { "r16g16_uint", vk::Format::eR16G16Uint },
            { "r16g16_sint", vk::Format::eR16G16Sint },
            { "r16g16_sfloat", vk::Format::eR16G16Sfloat },
            { "r16g16b16_unorm", vk::Format::eR16G16B16Unorm },
            { "r16g16b16_snorm", vk::Format::eR16G16B16Snorm },
            { "r16g16b16_uscaled", vk::Format::eR16G16B16Uscaled },
            { "r16g16b16_sscaled", vk::Format::eR16G16B16Sscaled },
            { "r16g16b16_uint", vk::Format::eR16G16B16Uint },
            { "r16g16b16_sint", vk::Format::eR16G16B16Sint },
            { "r16g16b16_sfloat", vk::Format::eR16G16B16Sfloat },
            { "r16g16b16a16_unorm", vk::Format::eR16G16B16A16Unorm },
            { "r16g16b16a16_snorm", vk::Format::eR16G16B16A16Snorm },
            { "r16g16b16a16_uscaled", vk::Format::eR16G16B16A16Uscaled },
            { "r16g16b16a16_sscaled", vk::Format::eR16G16B16A16Sscaled },
            { "r16g16b16a16_uint", vk::Format::eR16G16B16A16Uint },
            { "r16g16b16a16_sint", vk::Format::eR16G16B16A16Sint },
            { "r16g16b16a16_sfloat", vk::Format::eR16G16B16A16Sfloat },
            { "r32_uint", vk::Format::eR32Uint },
            { "r32_sint", vk::Format::eR32Sint },
            { "r32_sfloat", vk::Format::eR32Sfloat },
            { "r32g32_uint", vk::Format::eR32G32Uint },
            { "r32g32_sint", vk::Format::eR32G32Sint },
            { "r32g32_sfloat", vk::Format::eR32G32Sfloat },
            { "r32g32b32_uint", vk::Format::eR32G32B32Uint },
            { "r32g32b32_sint", vk::Format::eR32G32B32Sint },
            { "r32g32b32_sfloat", vk::Format::eR32G32B32Sfloat },
            { "r32g32b32a32_uint", vk::Format::eR32G32B32A32Uint },
            { "r32g32b32a32_sint", vk::Format::eR32G32B32A32Sint },
            { "r32g32b32a32_sfloat", vk::Format::eR32G32B32A32Sfloat },
            { "r64_uint", vk::Format::eR64Uint },
            { "r64_sint", vk::Format::eR64Sint },
            { "r64_sfloat", vk::Format::eR64Sfloat },
            { "r64g64_uint", vk::Format::eR64G64Uint },
            { "r64g64_sint", vk::Format::eR64G64Sint },
            { "r64g64_sfloat", vk::Format::eR64G64Sfloat },
            { "r64g64b64_uint", vk::Format::eR64G64B64Uint },
            { "r64g64b64_sint", vk::Format::eR64G64B64Sint },
            { "r64g64b64_sfloat", vk::Format::eR64G64B64Sfloat },
            { "r64g64b64a64_uint", vk::Format::eR64G64B64A64Uint },
            { "r64g64b64a64_sint", vk::Format::eR64G64B64A64Sint },
            { "r64g64b64a64_sfloat", vk::Format::eR64G64B64A64Sfloat },
            { "b10g11r11_ufloat_pack32", vk::Format::eB10G11R11UfloatPack32 },
            { "e5b9g9r9_ufloat_pack32", vk::Format::eE5B9G9R9UfloatPack32 },
            { "d16_unorm", vk::Format::eD16Unorm },
            { "x8_d24_unorm_pack32", vk::Format::eX8D24UnormPack32 },
            { "d32_sfloat", vk::Format::eD32Sfloat },
            { "s8_uint", vk::Format::eS8Uint },
            { "d16_unorm_s8_uint", vk::Format::eD16UnormS8Uint },
            { "d24_unorm_s8_uint", vk::Format::eD24UnormS8Uint },
            { "d32_sfloat_s8_uint", vk::Format::eD32SfloatS8Uint },
            { "bc1_rgb_unorm_block", vk::Format::eBc1RgbUnormBlock },
            { "bc1_rgb_srgb_block", vk::Format::eBc1RgbSrgbBlock },
            { "bc1_rgba_unorm_block", vk::Format::eBc1RgbaUnormBlock },
            { "bc1_rgba_srgb_block", vk::Format::eBc1RgbaSrgbBlock },
            { "bc2_unorm_block", vk::Format::eBc2UnormBlock },
            { "bc2_srgb_block", vk::Format::eBc2SrgbBlock },
            { "bc3_unorm_block", vk::Format::eBc3UnormBlock },
            { "bc3_srgb_block", vk::Format::eBc3SrgbBlock },
            { "bc4_unorm_block", vk::Format::eBc4UnormBlock },
            { "bc4_snorm_block", vk::Format::eBc4SnormBlock },
            { "bc5_unorm_block", vk::Format::eBc5UnormBlock },
            { "bc5_snorm_block", vk::Format::eBc5SnormBlock },
            { "bc6h_ufloat_block", vk::Format::eBc6HUfloatBlock },
            { "bc6h_sfloat_block", vk::Format::eBc6HSfloatBlock },
            { "bc7_unorm_block", vk::Format::eBc7UnormBlock },
            { "bc7_srgb_block", vk::Format::eBc7SrgbBlock },
            { "etc2_r8g8b8_unorm_block", vk::Format::eEtc2R8G8B8UnormBlock },
            { "etc2_r8g8b8_srgb_block", vk::Format::eEtc2R8G8B8SrgbBlock },
            { "etc2_r8g8b8a1_unorm_block", vk::Format::eEtc2R8G8B8A1UnormBlock },
            { "etc2_r8g8b8a1_srgb_block", vk::Format::eEtc2R8G8B8A1SrgbBlock },
            { "etc2_r8g8b8a8_unorm_block", vk::Format::eEtc2R8G8B8A8UnormBlock },
            { "etc2_r8g8b8a8_srgb_block", vk::Format::eEtc2R8G8B8A8SrgbBlock },
            { "eac_r11_unorm_block", vk::Format::eEacR11UnormBlock },
            { "eac_r11_snorm_block", vk::Format::eEacR11SnormBlock },
            { "eac_r11g11_unorm_block", vk::Format::eEacR11G11UnormBlock },
            { "eac_r11g11_snorm_block", vk::Format::eEacR11G11SnormBlock },
            { "astc_4x4_unorm_block", vk::Format::eAstc4x4UnormBlock },
            { "astc_4x4_srgb_block", vk::Format::eAstc4x4SrgbBlock },
            { "astc_5x4_unorm_block", vk::Format::eAstc5x4UnormBlock },
            { "astc_5x4_srgb_block", vk::Format::eAstc5x4SrgbBlock },
            { "astc_5x5_unorm_block", vk::Format::eAstc5x5UnormBlock },
            { "astc_5x5_srgb_block", vk::Format::eAstc5x5SrgbBlock },
            { "astc_6x5_unorm_block", vk::Format::eAstc6x5UnormBlock },
            { "astc_6x5_srgb_block", vk::Format::eAstc6x5SrgbBlock },
            { "astc_6x6_unorm_block", vk::Format::eAstc6x6UnormBlock },
            { "astc_6x6_srgb_block", vk::Format::eAstc6x6SrgbBlock },
            { "astc_8x5_unorm_block", vk::Format::eAstc8x5UnormBlock },
            { "astc_8x5_srgb_block", vk::Format::eAstc8x5SrgbBlock },
            { "astc_8x6_unorm_block", vk::Format::eAstc8x6UnormBlock },
            { "astc_8x6_srgb_block", vk::Format::eAstc8x6SrgbBlock },
            { "astc_8x8_unorm_block", vk::Format::eAstc8x8UnormBlock },
            { "astc_8x8_srgb_block", vk::Format::eAstc8x8SrgbBlock },
            { "astc_10x5_unorm_block", vk::Format::eAstc10x5UnormBlock },
            { "astc_10x5_srgb_block", vk::Format::eAstc10x5SrgbBlock },
            { "astc_10x6_unorm_block", vk::Format::eAstc10x6UnormBlock },
            { "astc_10x6_srgb_block", vk::Format::eAstc10x6SrgbBlock },
            { "astc_10x8_unorm_block", vk::Format::eAstc10x8UnormBlock },
            { "astc_10x8_srgb_block", vk::Format::eAstc10x8SrgbBlock },
            { "astc_10x10_unorm_block", vk::Format::eAstc10x10UnormBlock },
            { "astc_10x10_srgb_block", vk::Format::eAstc10x10SrgbBlock },
            { "astc_12x10_unorm_block", vk::Format::eAstc12x10UnormBlock },
            { "astc_12x10_srgb_block", vk::Format::eAstc12x10SrgbBlock },
            { "astc_12x12_unorm_block", vk::Format::eAstc12x12UnormBlock },
            { "astc_12x12_srgb_block", vk::Format::eAstc12x12SrgbBlock },
            { "g8b8g8r8_422_unorm", vk::Format::eG8B8G8R8422Unorm },
            { "b8g8r8g8_422_unorm", vk::Format::eB8G8R8G8422Unorm },
            { "g8_b8_r8_3plane_420_unorm", vk::Format::eG8B8R83Plane420Unorm },
            { "g8_b8r8_2plane_420_unorm", vk::Format::eG8B8R82Plane420Unorm },
            { "g8_b8_r8_3plane_422_unorm", vk::Format::eG8B8R83Plane422Unorm },
            { "g8_b8r8_2plane_422_unorm", vk::Format::eG8B8R82Plane422Unorm },
            { "g8_b8_r8_3plane_444_unorm", vk::Format::eG8B8R83Plane444Unorm },
            { "r10x6_unorm_pack16", vk::Format::eR10X6UnormPack16 },
            { "r10x6g10x6_unorm_2pack16", vk::Format::eR10X6G10X6Unorm2Pack16 },
            { "r10x6g10x6b10x6a10x6_unorm_4pack16", vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16 },
            { "g10x6b10x6g10x6r10x6_422_unorm_4pack16", vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16 },
            { "b10x6g10x6r10x6g10x6_422_unorm_4pack16", vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16 },
            { "g10x6_b10x6_r10x6_3plane_420_unorm_3pack16", vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16 },
            { "g10x6_b10x6r10x6_2plane_420_unorm_3pack16", vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16 },
            { "g10x6_b10x6_r10x6_3plane_422_unorm_3pack16", vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16 },
            { "g10x6_b10x6r10x6_2plane_422_unorm_3pack16", vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16 },
            { "g10x6_b10x6_r10x6_3plane_444_unorm_3pack16", vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16 },
            { "r12x4_unorm_pack16", vk::Format::eR12X4UnormPack16 },
            { "r12x4g12x4_unorm_2pack16", vk::Format::eR12X4G12X4Unorm2Pack16 },
            { "r12x4g12x4b12x4a12x4_unorm_4pack16", vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16 },
            { "g12x4b12x4g12x4r12x4_422_unorm_4pack16", vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16 },
            { "b12x4g12x4r12x4g12x4_422_unorm_4pack16", vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16 },
            { "g12x4_b12x4_r12x4_3plane_420_unorm_3pack16", vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16 },
            { "g12x4_b12x4r12x4_2plane_420_unorm_3pack16", vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16 },
            { "g12x4_b12x4_r12x4_3plane_422_unorm_3pack16", vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16 },
            { "g12x4_b12x4r12x4_2plane_422_unorm_3pack16", vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16 },
            { "g12x4_b12x4_r12x4_3plane_444_unorm_3pack16", vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16 },
            { "g16b16g16r16_422_unorm", vk::Format::eG16B16G16R16422Unorm },
            { "b16g16r16g16_422_unorm", vk::Format::eB16G16R16G16422Unorm },
            { "g16_b16_r16_3plane_420_unorm", vk::Format::eG16B16R163Plane420Unorm },
            { "g16_b16r16_2plane_420_unorm", vk::Format::eG16B16R162Plane420Unorm },
            { "g16_b16_r16_3plane_422_unorm", vk::Format::eG16B16R163Plane422Unorm },
            { "g16_b16r16_2plane_422_unorm", vk::Format::eG16B16R162Plane422Unorm },
            { "g16_b16_r16_3plane_444_unorm", vk::Format::eG16B16R163Plane444Unorm },
            { "pvrtc1_2bpp_unorm_block_img", vk::Format::ePvrtc12BppUnormBlockIMG },
            { "pvrtc1_4bpp_unorm_block_img", vk::Format::ePvrtc14BppUnormBlockIMG },
            { "pvrtc2_2bpp_unorm_block_img", vk::Format::ePvrtc22BppUnormBlockIMG },
            { "pvrtc2_4bpp_unorm_block_img", vk::Format::ePvrtc24BppUnormBlockIMG },
            { "pvrtc1_2bpp_srgb_block_img", vk::Format::ePvrtc12BppSrgbBlockIMG },
            { "pvrtc1_4bpp_srgb_block_img", vk::Format::ePvrtc14BppSrgbBlockIMG },
            { "pvrtc2_2bpp_srgb_block_img", vk::Format::ePvrtc22BppSrgbBlockIMG },
            { "pvrtc2_4bpp_srgb_block_img", vk::Format::ePvrtc24BppSrgbBlockIMG },
            { "astc_4x4_sfloat_block_ext", vk::Format::eAstc4x4SfloatBlockEXT },
            { "astc_5x4_sfloat_block_ext", vk::Format::eAstc5x4SfloatBlockEXT },
            { "astc_5x5_sfloat_block_ext", vk::Format::eAstc5x5SfloatBlockEXT },
            { "astc_6x5_sfloat_block_ext", vk::Format::eAstc6x5SfloatBlockEXT },
            { "astc_6x6_sfloat_block_ext", vk::Format::eAstc6x6SfloatBlockEXT },
            { "astc_8x5_sfloat_block_ext", vk::Format::eAstc8x5SfloatBlockEXT },
            { "astc_8x6_sfloat_block_ext", vk::Format::eAstc8x6SfloatBlockEXT },
            { "astc_8x8_sfloat_block_ext", vk::Format::eAstc8x8SfloatBlockEXT },
            { "astc_10x5_sfloat_block_ext", vk::Format::eAstc10x5SfloatBlockEXT },
            { "astc_10x6_sfloat_block_ext", vk::Format::eAstc10x6SfloatBlockEXT },
            { "astc_10x8_sfloat_block_ext", vk::Format::eAstc10x8SfloatBlockEXT },
            { "astc_10x10_sfloat_block_ext", vk::Format::eAstc10x10SfloatBlockEXT },
            { "astc_12x10_sfloat_block_ext", vk::Format::eAstc12x10SfloatBlockEXT },
            { "astc_12x12_sfloat_block_ext", vk::Format::eAstc12x12SfloatBlockEXT },
            { "g8_b8r8_2plane_444_unorm_ext", vk::Format::eG8B8R82Plane444UnormEXT },
            { "g10x6_b10x6r10x6_2plane_444_unorm_3pack16_ext", vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16EXT },
            { "g12x4_b12x4r12x4_2plane_444_unorm_3pack16_ext", vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16EXT },
            { "g16_b16r16_2plane_444_unorm_ext", vk::Format::eG16B16R162Plane444UnormEXT },
            { "a4r4g4b4_unorm_pack16_ext", vk::Format::eA4R4G4B4UnormPack16EXT },
            { "a4b4g4r4_unorm_pack16_ext", vk::Format::eA4B4G4R4UnormPack16EXT },
            { "b10x6g10x6r10x6g10x6_422_unorm_4pack16_khr", vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR },
            { "b12x4g12x4r12x4g12x4_422_unorm_4pack16_khr", vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR },
            { "b16g16r16g16_422_unorm_khr", vk::Format::eB16G16R16G16422UnormKHR },
            { "b8g8r8g8_422_unorm_khr", vk::Format::eB8G8R8G8422UnormKHR },
            { "g10x6b10x6g10x6r10x6_422_unorm_4pack16_khr", vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR },
            { "g10x6_b10x6r10x6_2plane_420_unorm_3pack16_khr", vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16KHR },
            { "g10x6_b10x6r10x6_2plane_422_unorm_3pack16_khr", vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16KHR },
            { "g10x6_b10x6_r10x6_3plane_420_unorm_3pack16_khr", vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16KHR },
            { "g10x6_b10x6_r10x6_3plane_422_unorm_3pack16_khr", vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16KHR },
            { "g10x6_b10x6_r10x6_3plane_444_unorm_3pack16_khr", vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16KHR },
            { "g12x4b12x4g12x4r12x4_422_unorm_4pack16_khr", vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR },
            { "g12x4_b12x4r12x4_2plane_420_unorm_3pack16_khr", vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16KHR },
            { "g12x4_b12x4r12x4_2plane_422_unorm_3pack16_khr", vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16KHR },
            { "g12x4_b12x4_r12x4_3plane_420_unorm_3pack16_khr", vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16KHR },
            { "g12x4_b12x4_r12x4_3plane_422_unorm_3pack16_khr", vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16KHR },
            { "g12x4_b12x4_r12x4_3plane_444_unorm_3pack16_khr", vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16KHR },
            { "g16b16g16r16_422_unorm_khr", vk::Format::eG16B16G16R16422UnormKHR },
            { "g16_b16r16_2plane_420_unorm_khr", vk::Format::eG16B16R162Plane420UnormKHR },
            { "g16_b16r16_2plane_422_unorm_khr", vk::Format::eG16B16R162Plane422UnormKHR },
            { "g16_b16_r16_3plane_420_unorm_khr", vk::Format::eG16B16R163Plane420UnormKHR },
            { "g16_b16_r16_3plane_422_unorm_khr", vk::Format::eG16B16R163Plane422UnormKHR },
            { "g16_b16_r16_3plane_444_unorm_khr", vk::Format::eG16B16R163Plane444UnormKHR },
            { "g8b8g8r8_422_unorm_khr", vk::Format::eG8B8G8R8422UnormKHR },
            { "g8_b8r8_2plane_420_unorm_khr", vk::Format::eG8B8R82Plane420UnormKHR },
            { "g8_b8r8_2plane_422_unorm_khr", vk::Format::eG8B8R82Plane422UnormKHR },
            { "g8_b8_r8_3plane_420_unorm_khr", vk::Format::eG8B8R83Plane420UnormKHR },
            { "g8_b8_r8_3plane_422_unorm_khr", vk::Format::eG8B8R83Plane422UnormKHR },
            { "g8_b8_r8_3plane_444_unorm_khr", vk::Format::eG8B8R83Plane444UnormKHR },
            { "r10x6g10x6b10x6a10x6_unorm_4pack16_khr", vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16KHR },
            { "r10x6g10x6_unorm_2pack16_khr", vk::Format::eR10X6G10X6Unorm2Pack16KHR },
            { "r10x6_unorm_pack16_khr", vk::Format::eR10X6UnormPack16KHR },
            { "r12x4g12x4b12x4a12x4_unorm_4pack16_khr", vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16KHR },
            { "r12x4g12x4_unorm_2pack16_khr", vk::Format::eR12X4G12X4Unorm2Pack16KHR },
            { "r12x4_unorm_pack16_khr", vk::Format::eR12X4UnormPack16KHR }
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

template <>
struct Json::Into<vk::VertexInputRate> {
    static auto into(const Json& o) -> std::optional<vk::VertexInputRate> {
        static const auto table = std::map<std::string, vk::VertexInputRate>{
            {"vertex", vk::VertexInputRate::eVertex},
            {"instance", vk::VertexInputRate::eInstance}
        };

        if (auto it = table.find(o.to_string()); it != table.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

struct membuf : std::streambuf {
    explicit membuf(std::span<const char> bytes) {
        setg((char*)bytes.data(), (char*)bytes.data(), (char*)bytes.data() + bytes.size());
    }
};

struct memstream : std::istream {
    explicit memstream(std::span<const char> bytes) : std::istream(&_buffer), _buffer(bytes) {
        rdbuf(&_buffer);
    }

private:
    membuf _buffer;
};

struct Material::Impl {
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;
    std::vector<uint32_t> dynamicOffsets;

    vk::DescriptorPool descriptorPool;
    vk::DescriptorSetLayout descriptorSetLayout;
    std::vector<vk::DescriptorSet> descriptorSets;

    std::vector<vk::Sampler> samplers;

    explicit Impl(const std::string& filename) {
        _createDescriptorPool();

        const auto material_resource = Resources::get(filename).value();
        const auto o = Json::Read::read(memstream{material_resource.bytes()}).value();

        auto constants = std::vector<vk::PushConstantRange>{};
        auto stages = std::vector<vk::PipelineShaderStageCreateInfo>{};
        auto bindings = std::vector<vk::VertexInputBindingDescription>{};
        auto attributes = std::vector<vk::VertexInputAttributeDescription>{};
        auto descriptorSetLayoutBindings = std::vector<vk::DescriptorSetLayoutBinding>{};

        for (auto&& stage : o.at("stages").to_array()) {
            const auto file = Resources::get(stage.at("file").to_string()).value();
            const auto data = reinterpret_cast<const uint32_t *>(file.data());

            assert((file.size() % 4) == 0);

            auto glsl = spirv_cross::CompilerGLSL(data, file.size() / 4);

            const auto moduleCreateInfo = vk::ShaderModuleCreateInfo{
                .codeSize = file.size(),
                .pCode    = data
            };

            const auto stageCreateInfo = vk::PipelineShaderStageCreateInfo{
                .flags  = {},
                .stage  = stage.at("type"),
                .module = Blaze::GetLogicalDevice().createShaderModule(moduleCreateInfo),
                .pName = glsl.get_entry_points_and_stages()[0].name.c_str()
            };
            stages.emplace_back(stageCreateInfo);

            auto resources = glsl.get_shader_resources();
            for (auto&& resource : resources.uniform_buffers) {
//                const auto set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
                const auto binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
                const auto descriptorSetLayoutBinding = vk::DescriptorSetLayoutBinding{
                    .binding = binding,
                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                    .descriptorCount = 1,
                    .stageFlags = stageCreateInfo.stage,
                    .pImmutableSamplers = nullptr
                };
                descriptorSetLayoutBindings.emplace_back(descriptorSetLayoutBinding);
            }

            for (auto&& resource : resources.sampled_images) {
//                const auto set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
                const auto binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
//                const auto type = glsl.get_type(resource.base_type_id);

                const auto descriptorSetLayoutBinding = vk::DescriptorSetLayoutBinding{
                    .binding = binding,
                    .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                    .descriptorCount = 1,
                    .stageFlags = stageCreateInfo.stage,
                    .pImmutableSamplers = nullptr
                };
                descriptorSetLayoutBindings.emplace_back(descriptorSetLayoutBinding);
            }

            if (!resources.push_constant_buffers.empty()) {
                assert(resources.push_constant_buffers.size() == 1);
                auto&& resource = resources.push_constant_buffers[0];
                auto&& type = glsl.get_type(resource.base_type_id);

//                const auto size = glsl.get_declared_struct_size(type);
//                spdlog::info("ConstantBuffer: name = {}, size = {}", resource.name, size);
//                for (auto&& range : glsl.get_active_buffer_ranges(resource.id)) {
//                    spdlog::info("Accessing member {}, offset {}, size {}", range.index, range.offset, range.range);
//                }

                constants.emplace_back(vk::PushConstantRange{
                    .stageFlags = stageCreateInfo.stage,
                    .offset     = 0,
                    .size       = static_cast<uint32_t>(glsl.get_declared_struct_size(type))
                });
            }

//            spdlog::info("");
        }

        const auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo {}
            .setBindings(descriptorSetLayoutBindings);

        descriptorSetLayout = Blaze::GetLogicalDevice().createDescriptorSetLayout(layoutCreateInfo, nullptr);

        const auto descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo {
            .descriptorPool = descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &descriptorSetLayout
        };

        descriptorSets = Blaze::GetLogicalDevice().allocateDescriptorSets(descriptorSetAllocateInfo);

        if (o.contains("bindings")) {
            for (auto&& binding : o.at("bindings").to_array()) {
                bindings.emplace_back(vk::VertexInputBindingDescription{
                    .binding   = static_cast<uint32_t>(bindings.size()),
                    .stride    = binding.at("stride"),
                    .inputRate = binding.at("input_rate")
                });

                for (auto&& attribute: binding.at("attributes").to_array()) {
                    attributes.emplace_back(vk::VertexInputAttributeDescription{
                        .location = attribute.at("location"),
                        .binding  = bindings.back().binding,
                        .format   = attribute.at("format"),
                        .offset   = attribute.at("offset")
                    });
                }
            }
        }
        const auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{}
            .setVertexBindingDescriptions(bindings)
            .setVertexAttributeDescriptions(attributes);

        const auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{
            .flags                  = {},
            .topology               = o.value_or("topology", vk::PrimitiveTopology::ePointList),
            .primitiveRestartEnable = o.value_or("primitive_restart_enable", false)
        };

        const auto viewportState = vk::PipelineViewportStateCreateInfo{
            .viewportCount = 1,
            .pViewports    = nullptr,
            .scissorCount  = 1,
            .pScissors     = nullptr
        };

        const auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{
            .flags                   = {},
            .depthClampEnable        = o.value_or("depth_clamp_enable", false),
            .rasterizerDiscardEnable = o.value_or("rasterizer_discard_enable", false),
            .polygonMode             = o.value_or("polygon_mode", vk::PolygonMode::eFill),
            .cullMode                = o.value_or("cull_mode", vk::CullModeFlags{}),
            .frontFace               = o.value_or("front_face", vk::FrontFace::eCounterClockwise),
            .depthBiasEnable         = o.value_or("depth_bias_enable", false),
            .depthBiasConstantFactor = o.value_or("depth_bias_constant_factor", 0.0f),
            .depthBiasClamp          = o.value_or("depth_bias_clamp", 0.0f),
            .depthBiasSlopeFactor    = o.value_or("depth_bias_slope_factor", 0.0f),
            .lineWidth               = o.value_or("line_width", 0.0f)
        };

        const auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};

        const auto depthStencilState = vk::PipelineDepthStencilStateCreateInfo{
            .flags                 = {},
            .depthTestEnable       = o.value_or("depth_test_enable", false),
            .depthWriteEnable      = o.value_or("depth_write_enable", false),
            .depthCompareOp        = o.value_or("depth_compare_op", vk::CompareOp::eNever),
            .depthBoundsTestEnable = o.value_or("depth_bounds_test_enable", false),
            .stencilTestEnable     = o.value_or("stencil_test_enable", false),
            .front                 = vk::StencilOpState{},
            .back                  = vk::StencilOpState{},
            .minDepthBounds        = o.value_or("min_depth_bounds", 0.0f),
            .maxDepthBounds        = o.value_or("max_depth_bounds", 0.0f)
        };

        auto attachments = std::vector<vk::PipelineColorBlendAttachmentState>{};
        if (o.contains("attachments")) {
            for (auto&& attachment : o.at("attachments").to_array()) {
                attachments.emplace_back(vk::PipelineColorBlendAttachmentState{
                    .blendEnable         = attachment.value_or("blend_enable", false),
                    .srcColorBlendFactor = attachment.value_or("src_color_blend_factor", vk::BlendFactor::eZero),
                    .dstColorBlendFactor = attachment.value_or("dst_color_blend_factor", vk::BlendFactor::eZero),
                    .colorBlendOp        = attachment.value_or("color_blend_op", vk::BlendOp::eAdd),
                    .srcAlphaBlendFactor = attachment.value_or("src_alpha_blend_factor", vk::BlendFactor::eZero),
                    .dstAlphaBlendFactor = attachment.value_or("dst_alpha_blend_factor", vk::BlendFactor::eZero),
                    .alphaBlendOp        = attachment.value_or("alpha_blend_op", vk::BlendOp::eAdd),
                    .colorWriteMask      = vk::ColorComponentFlagBits::eR
                                         | vk::ColorComponentFlagBits::eG
                                         | vk::ColorComponentFlagBits::eB
                                         | vk::ColorComponentFlagBits::eA
                });
            }
        }

        const auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{}
            .setAttachments(attachments);

        const auto dynamicStates = std::array{
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        const auto dynamicState = vk::PipelineDynamicStateCreateInfo{}
            .setDynamicStates(dynamicStates);

        const auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo{}
            .setSetLayouts(descriptorSetLayout)
            .setPushConstantRanges(constants);

        pipelineLayout = Blaze::GetLogicalDevice().createPipelineLayout(pipelineLayoutCreateInfo);

        const auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{
            .pVertexInputState = &vertexInputState,
            .pInputAssemblyState = &inputAssemblyState,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizationState,
            .pMultisampleState = &multisampleState,
            .pDepthStencilState = &depthStencilState,
            .pColorBlendState = &colorBlendState,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout,
            .renderPass = Blaze::GetSwapchainRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE
        }.setStages(stages);

        Blaze::GetLogicalDevice().createGraphicsPipelines(nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline);

        for (auto &stage: stages) {
            Blaze::GetLogicalDevice().destroyShaderModule(stage.module);
        }
    }

    ~Impl() {
        Blaze::GetLogicalDevice().destroyDescriptorSetLayout(descriptorSetLayout, nullptr);
        Blaze::GetLogicalDevice().destroyDescriptorPool(descriptorPool, nullptr);
        Blaze::GetLogicalDevice().destroyPipelineLayout(pipelineLayout, nullptr);
        Blaze::GetLogicalDevice().destroyPipeline(pipeline, nullptr);

    }

    void _createDescriptorPool() {
        const auto poolSizes = std::array {
            vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1},
        };
        const auto descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo{
            .maxSets = 1,
            .poolSizeCount = static_cast<uint32_t>(std::size(poolSizes)),
            .pPoolSizes = std::data(poolSizes)
        };
        descriptorPool = Blaze::GetLogicalDevice().createDescriptorPool(descriptorPoolCreateInfo, nullptr);
    }
};

auto Material::LoadFromResources(const std::string& filename) -> Material {
    Material material{};
    material.impl = blaze::make_internal<Impl>(filename);
    return material;
}

auto Material::getPipeline() const -> vk::Pipeline {
    return impl->pipeline;
}

auto Material::getPipelineLayout() const -> vk::PipelineLayout {
    return impl->pipelineLayout;
}

auto Material::getDynamicOffsets() const -> std::span<const uint32_t> {
    return impl->dynamicOffsets;
}

auto Material::getDescriptorSets() const -> std::span<const vk::DescriptorSet> {
    return impl->descriptorSets;
}

void Material::SetConstantBuffer(uint32_t index, const GraphicsBuffer& buffer) {
    auto vk_buffer = static_cast<VulkanGraphicsBuffer*>(buffer.getNativeBufferPtr());

    const auto bufferInfo = vk::DescriptorBufferInfo {
        .buffer = vk_buffer->buffer,
        .offset = 0,
        .range = static_cast<vk::DeviceSize>(buffer.getSize())
    };
    const auto writeDescriptorSet = vk::WriteDescriptorSet{
        .dstSet = impl->descriptorSets[0],
        .dstBinding = index,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo = &bufferInfo
    };

    Blaze::GetLogicalDevice().updateDescriptorSets({writeDescriptorSet}, {});
}

void Material::SetTexture(uint32_t index, const Texture2D &texture) {
    const auto imageInfo = vk::DescriptorImageInfo{
        .sampler = texture.getSampler(),
        .imageView = texture.getImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
    };

    const auto writeDescriptorSet = vk::WriteDescriptorSet{
        .dstSet = impl->descriptorSets[0],
        .dstBinding = index,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo = &imageInfo
    };

    Blaze::GetLogicalDevice().updateDescriptorSets({writeDescriptorSet}, {});
}
