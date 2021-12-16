#include "Texture.hpp"
#include "Graphics.hpp"
#include "CommandPool.hpp"
#include "GraphicsBuffer.hpp"
#include "Blaze.hpp"

#include <map>
#include <VulkanTexture.hpp>
#include <VulkanGfxDevice.hpp>
#include <VulkanGraphicsBuffer.hpp>

extern auto GetGfxDevice() -> VulkanGfxDevice&;

struct VULKAN_FORMAT_INFO {
    int size;
    int channels;
//    VkFormatCompatibilityClass format_class;
};

const auto vk_format_table = std::map<vk::Format, VULKAN_FORMAT_INFO>{
//    { vk::Format::eUndefined,                                { 0, 0 } },
//    { vk::Format::eR4G4UnormPack8,                           { 0, 0 } },
//    { vk::Format::eR4G4B4A4UnormPack16,                      { 0, 0 } },
//    { vk::Format::eB4G4R4A4UnormPack16,                      { 0, 0 } },
//    { vk::Format::eR5G6B5UnormPack16,                        { 0, 0 } },
//    { vk::Format::eB5G6R5UnormPack16,                        { 0, 0 } },
//    { vk::Format::eR5G5B5A1UnormPack16,                      { 0, 0 } },
//    { vk::Format::eB5G5R5A1UnormPack16,                      { 0, 0 } },
//    { vk::Format::eA1R5G5B5UnormPack16,                      { 0, 0 } },
//    { vk::Format::eR8Unorm,                                  { 0, 0 } },
//    { vk::Format::eR8Snorm,                                  { 0, 0 } },
//    { vk::Format::eR8Uscaled,                                { 0, 0 } },
//    { vk::Format::eR8Sscaled,                                { 0, 0 } },
//    { vk::Format::eR8Uint,                                   { 0, 0 } },
//    { vk::Format::eR8Sint,                                   { 0, 0 } },
//    { vk::Format::eR8Srgb,                                   { 0, 0 } },
//    { vk::Format::eR8G8Unorm,                                { 0, 0 } },
//    { vk::Format::eR8G8Snorm,                                { 0, 0 } },
//    { vk::Format::eR8G8Uscaled,                              { 0, 0 } },
//    { vk::Format::eR8G8Sscaled,                              { 0, 0 } },
//    { vk::Format::eR8G8Uint,                                 { 0, 0 } },
//    { vk::Format::eR8G8Sint,                                 { 0, 0 } },
//    { vk::Format::eR8G8Srgb,                                 { 0, 0 } },
//    { vk::Format::eR8G8B8Unorm,                              { 0, 0 } },
//    { vk::Format::eR8G8B8Snorm,                              { 0, 0 } },
//    { vk::Format::eR8G8B8Uscaled,                            { 0, 0 } },
//    { vk::Format::eR8G8B8Sscaled,                            { 0, 0 } },
//    { vk::Format::eR8G8B8Uint,                               { 0, 0 } },
//    { vk::Format::eR8G8B8Sint,                               { 0, 0 } },
//    { vk::Format::eR8G8B8Srgb,                               { 0, 0 } },
//    { vk::Format::eB8G8R8Unorm,                              { 0, 0 } },
//    { vk::Format::eB8G8R8Snorm,                              { 0, 0 } },
//    { vk::Format::eB8G8R8Uscaled,                            { 0, 0 } },
//    { vk::Format::eB8G8R8Sscaled,                            { 0, 0 } },
//    { vk::Format::eB8G8R8Uint,                               { 0, 0 } },
//    { vk::Format::eB8G8R8Sint,                               { 0, 0 } },
//    { vk::Format::eB8G8R8Srgb,                               { 0, 0 } },
    { vk::Format::eR8G8B8A8Unorm,                            { 0, 4 } },
    { vk::Format::eR8G8B8A8Snorm,                            { 0, 4 } },
    { vk::Format::eR8G8B8A8Uscaled,                          { 0, 4 } },
    { vk::Format::eR8G8B8A8Sscaled,                          { 0, 4 } },
    { vk::Format::eR8G8B8A8Uint,                             { 0, 4 } },
    { vk::Format::eR8G8B8A8Sint,                             { 0, 4 } },
    { vk::Format::eR8G8B8A8Srgb,                             { 0, 4 } },
//    { vk::Format::eB8G8R8A8Unorm,                            { 0, 0 } },
//    { vk::Format::eB8G8R8A8Snorm,                            { 0, 0 } },
//    { vk::Format::eB8G8R8A8Uscaled,                          { 0, 0 } },
//    { vk::Format::eB8G8R8A8Sscaled,                          { 0, 0 } },
//    { vk::Format::eB8G8R8A8Uint,                             { 0, 0 } },
//    { vk::Format::eB8G8R8A8Sint,                             { 0, 0 } },
//    { vk::Format::eB8G8R8A8Srgb,                             { 0, 0 } },
//    { vk::Format::eA8B8G8R8UnormPack32,                      { 0, 0 } },
//    { vk::Format::eA8B8G8R8SnormPack32,                      { 0, 0 } },
//    { vk::Format::eA8B8G8R8UscaledPack32,                    { 0, 0 } },
//    { vk::Format::eA8B8G8R8SscaledPack32,                    { 0, 0 } },
//    { vk::Format::eA8B8G8R8UintPack32,                       { 0, 0 } },
//    { vk::Format::eA8B8G8R8SintPack32,                       { 0, 0 } },
//    { vk::Format::eA8B8G8R8SrgbPack32,                       { 0, 0 } },
//    { vk::Format::eA2R10G10B10UnormPack32,                   { 0, 0 } },
//    { vk::Format::eA2R10G10B10SnormPack32,                   { 0, 0 } },
//    { vk::Format::eA2R10G10B10UscaledPack32,                 { 0, 0 } },
//    { vk::Format::eA2R10G10B10SscaledPack32,                 { 0, 0 } },
//    { vk::Format::eA2R10G10B10UintPack32,                    { 0, 0 } },
//    { vk::Format::eA2R10G10B10SintPack32,                    { 0, 0 } },
//    { vk::Format::eA2B10G10R10UnormPack32,                   { 0, 0 } },
//    { vk::Format::eA2B10G10R10SnormPack32,                   { 0, 0 } },
//    { vk::Format::eA2B10G10R10UscaledPack32,                 { 0, 0 } },
//    { vk::Format::eA2B10G10R10SscaledPack32,                 { 0, 0 } },
//    { vk::Format::eA2B10G10R10UintPack32,                    { 0, 0 } },
//    { vk::Format::eA2B10G10R10SintPack32,                    { 0, 0 } },
//    { vk::Format::eR16Unorm,                                 { 0, 0 } },
//    { vk::Format::eR16Snorm,                                 { 0, 0 } },
//    { vk::Format::eR16Uscaled,                               { 0, 0 } },
//    { vk::Format::eR16Sscaled,                               { 0, 0 } },
//    { vk::Format::eR16Uint,                                  { 0, 0 } },
//    { vk::Format::eR16Sint,                                  { 0, 0 } },
//    { vk::Format::eR16Sfloat,                                { 0, 0 } },
//    { vk::Format::eR16G16Unorm,                              { 0, 0 } },
//    { vk::Format::eR16G16Snorm,                              { 0, 0 } },
//    { vk::Format::eR16G16Uscaled,                            { 0, 0 } },
//    { vk::Format::eR16G16Sscaled,                            { 0, 0 } },
//    { vk::Format::eR16G16Uint,                               { 0, 0 } },
//    { vk::Format::eR16G16Sint,                               { 0, 0 } },
//    { vk::Format::eR16G16Sfloat,                             { 0, 0 } },
//    { vk::Format::eR16G16B16Unorm,                           { 0, 0 } },
//    { vk::Format::eR16G16B16Snorm,                           { 0, 0 } },
//    { vk::Format::eR16G16B16Uscaled,                         { 0, 0 } },
//    { vk::Format::eR16G16B16Sscaled,                         { 0, 0 } },
//    { vk::Format::eR16G16B16Uint,                            { 0, 0 } },
//    { vk::Format::eR16G16B16Sint,                            { 0, 0 } },
//    { vk::Format::eR16G16B16Sfloat,                          { 0, 0 } },
//    { vk::Format::eR16G16B16A16Unorm,                        { 0, 0 } },
//    { vk::Format::eR16G16B16A16Snorm,                        { 0, 0 } },
//    { vk::Format::eR16G16B16A16Uscaled,                      { 0, 0 } },
//    { vk::Format::eR16G16B16A16Sscaled,                      { 0, 0 } },
//    { vk::Format::eR16G16B16A16Uint,                         { 0, 0 } },
//    { vk::Format::eR16G16B16A16Sint,                         { 0, 0 } },
//    { vk::Format::eR16G16B16A16Sfloat,                       { 0, 0 } },
//    { vk::Format::eR32Uint,                                  { 0, 0 } },
//    { vk::Format::eR32Sint,                                  { 0, 0 } },
//    { vk::Format::eR32Sfloat,                                { 0, 0 } },
//    { vk::Format::eR32G32Uint,                               { 0, 0 } },
//    { vk::Format::eR32G32Sint,                               { 0, 0 } },
//    { vk::Format::eR32G32Sfloat,                             { 0, 0 } },
//    { vk::Format::eR32G32B32Uint,                            { 0, 0 } },
//    { vk::Format::eR32G32B32Sint,                            { 0, 0 } },
//    { vk::Format::eR32G32B32Sfloat,                          { 0, 0 } },
//    { vk::Format::eR32G32B32A32Uint,                         { 0, 0 } },
//    { vk::Format::eR32G32B32A32Sint,                         { 0, 0 } },
//    { vk::Format::eR32G32B32A32Sfloat,                       { 0, 0 } },
//    { vk::Format::eR64Uint,                                  { 0, 0 } },
//    { vk::Format::eR64Sint,                                  { 0, 0 } },
//    { vk::Format::eR64Sfloat,                                { 0, 0 } },
//    { vk::Format::eR64G64Uint,                               { 0, 0 } },
//    { vk::Format::eR64G64Sint,                               { 0, 0 } },
//    { vk::Format::eR64G64Sfloat,                             { 0, 0 } },
//    { vk::Format::eR64G64B64Uint,                            { 0, 0 } },
//    { vk::Format::eR64G64B64Sint,                            { 0, 0 } },
//    { vk::Format::eR64G64B64Sfloat,                          { 0, 0 } },
//    { vk::Format::eR64G64B64A64Uint,                         { 0, 0 } },
//    { vk::Format::eR64G64B64A64Sint,                         { 0, 0 } },
//    { vk::Format::eR64G64B64A64Sfloat,                       { 0, 0 } },
//    { vk::Format::eB10G11R11UfloatPack32,                    { 0, 0 } },
//    { vk::Format::eE5B9G9R9UfloatPack32,                     { 0, 0 } },
//    { vk::Format::eD16Unorm,                                 { 0, 0 } },
//    { vk::Format::eX8D24UnormPack32,                         { 0, 0 } },
//    { vk::Format::eD32Sfloat,                                { 0, 0 } },
//    { vk::Format::eS8Uint,                                   { 0, 0 } },
//    { vk::Format::eD16UnormS8Uint,                           { 0, 0 } },
//    { vk::Format::eD24UnormS8Uint,                           { 0, 0 } },
//    { vk::Format::eD32SfloatS8Uint,                          { 0, 0 } },
//    { vk::Format::eBc1RgbUnormBlock,                         { 0, 0 } },
//    { vk::Format::eBc1RgbSrgbBlock,                          { 0, 0 } },
//    { vk::Format::eBc1RgbaUnormBlock,                        { 0, 0 } },
//    { vk::Format::eBc1RgbaSrgbBlock,                         { 0, 0 } },
//    { vk::Format::eBc2UnormBlock,                            { 0, 0 } },
//    { vk::Format::eBc2SrgbBlock,                             { 0, 0 } },
//    { vk::Format::eBc3UnormBlock,                            { 0, 0 } },
//    { vk::Format::eBc3SrgbBlock,                             { 0, 0 } },
//    { vk::Format::eBc4UnormBlock,                            { 0, 0 } },
//    { vk::Format::eBc4SnormBlock,                            { 0, 0 } },
//    { vk::Format::eBc5UnormBlock,                            { 0, 0 } },
//    { vk::Format::eBc5SnormBlock,                            { 0, 0 } },
//    { vk::Format::eBc6HUfloatBlock,                          { 0, 0 } },
//    { vk::Format::eBc6HSfloatBlock,                          { 0, 0 } },
//    { vk::Format::eBc7UnormBlock,                            { 0, 0 } },
//    { vk::Format::eBc7SrgbBlock,                             { 0, 0 } },
//    { vk::Format::eEtc2R8G8B8UnormBlock,                     { 0, 0 } },
//    { vk::Format::eEtc2R8G8B8SrgbBlock,                      { 0, 0 } },
//    { vk::Format::eEtc2R8G8B8A1UnormBlock,                   { 0, 0 } },
//    { vk::Format::eEtc2R8G8B8A1SrgbBlock,                    { 0, 0 } },
//    { vk::Format::eEtc2R8G8B8A8UnormBlock,                   { 0, 0 } },
//    { vk::Format::eEtc2R8G8B8A8SrgbBlock,                    { 0, 0 } },
//    { vk::Format::eEacR11UnormBlock,                         { 0, 0 } },
//    { vk::Format::eEacR11SnormBlock,                         { 0, 0 } },
//    { vk::Format::eEacR11G11UnormBlock,                      { 0, 0 } },
//    { vk::Format::eEacR11G11SnormBlock,                      { 0, 0 } },
//    { vk::Format::eAstc4x4UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc4x4SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc5x4UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc5x4SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc5x5UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc5x5SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc6x5UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc6x5SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc6x6UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc6x6SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc8x5UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc8x5SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc8x6UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc8x6SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc8x8UnormBlock,                        { 0, 0 } },
//    { vk::Format::eAstc8x8SrgbBlock,                         { 0, 0 } },
//    { vk::Format::eAstc10x5UnormBlock,                       { 0, 0 } },
//    { vk::Format::eAstc10x5SrgbBlock,                        { 0, 0 } },
//    { vk::Format::eAstc10x6UnormBlock,                       { 0, 0 } },
//    { vk::Format::eAstc10x6SrgbBlock,                        { 0, 0 } },
//    { vk::Format::eAstc10x8UnormBlock,                       { 0, 0 } },
//    { vk::Format::eAstc10x8SrgbBlock,                        { 0, 0 } },
//    { vk::Format::eAstc10x10UnormBlock,                      { 0, 0 } },
//    { vk::Format::eAstc10x10SrgbBlock,                       { 0, 0 } },
//    { vk::Format::eAstc12x10UnormBlock,                      { 0, 0 } },
//    { vk::Format::eAstc12x10SrgbBlock,                       { 0, 0 } },
//    { vk::Format::eAstc12x12UnormBlock,                      { 0, 0 } },
//    { vk::Format::eAstc12x12SrgbBlock,                       { 0, 0 } },
//    { vk::Format::eG8B8G8R8422Unorm,                         { 0, 0 } },
//    { vk::Format::eB8G8R8G8422Unorm,                         { 0, 0 } },
//    { vk::Format::eG8B8R83Plane420Unorm,                     { 0, 0 } },
//    { vk::Format::eG8B8R82Plane420Unorm,                     { 0, 0 } },
//    { vk::Format::eG8B8R83Plane422Unorm,                     { 0, 0 } },
//    { vk::Format::eG8B8R82Plane422Unorm,                     { 0, 0 } },
//    { vk::Format::eG8B8R83Plane444Unorm,                     { 0, 0 } },
//    { vk::Format::eR10X6UnormPack16,                         { 0, 0 } },
//    { vk::Format::eR10X6G10X6Unorm2Pack16,                   { 0, 0 } },
//    { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16,         { 0, 0 } },
//    { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16,      { 0, 0 } },
//    { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16,      { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eR12X4UnormPack16,                         { 0, 0 } },
//    { vk::Format::eR12X4G12X4Unorm2Pack16,                   { 0, 0 } },
//    { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16,         { 0, 0 } },
//    { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16,      { 0, 0 } },
//    { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16,      { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16,     { 0, 0 } },
//    { vk::Format::eG16B16G16R16422Unorm,                     { 0, 0 } },
//    { vk::Format::eB16G16R16G16422Unorm,                     { 0, 0 } },
//    { vk::Format::eG16B16R163Plane420Unorm,                  { 0, 0 } },
//    { vk::Format::eG16B16R162Plane420Unorm,                  { 0, 0 } },
//    { vk::Format::eG16B16R163Plane422Unorm,                  { 0, 0 } },
//    { vk::Format::eG16B16R162Plane422Unorm,                  { 0, 0 } },
//    { vk::Format::eG16B16R163Plane444Unorm,                  { 0, 0 } },
//    { vk::Format::ePvrtc12BppUnormBlockIMG,                  { 0, 0 } },
//    { vk::Format::ePvrtc14BppUnormBlockIMG,                  { 0, 0 } },
//    { vk::Format::ePvrtc22BppUnormBlockIMG,                  { 0, 0 } },
//    { vk::Format::ePvrtc24BppUnormBlockIMG,                  { 0, 0 } },
//    { vk::Format::ePvrtc12BppSrgbBlockIMG,                   { 0, 0 } },
//    { vk::Format::ePvrtc14BppSrgbBlockIMG,                   { 0, 0 } },
//    { vk::Format::ePvrtc22BppSrgbBlockIMG,                   { 0, 0 } },
//    { vk::Format::ePvrtc24BppSrgbBlockIMG,                   { 0, 0 } },
//    { vk::Format::eAstc4x4SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc5x4SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc5x5SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc6x5SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc6x6SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc8x5SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc8x6SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc8x8SfloatBlockEXT,                    { 0, 0 } },
//    { vk::Format::eAstc10x5SfloatBlockEXT,                   { 0, 0 } },
//    { vk::Format::eAstc10x6SfloatBlockEXT,                   { 0, 0 } },
//    { vk::Format::eAstc10x8SfloatBlockEXT,                   { 0, 0 } },
//    { vk::Format::eAstc10x10SfloatBlockEXT,                  { 0, 0 } },
//    { vk::Format::eAstc12x10SfloatBlockEXT,                  { 0, 0 } },
//    { vk::Format::eAstc12x12SfloatBlockEXT,                  { 0, 0 } },
//    { vk::Format::eG8B8R82Plane444UnormEXT,                  { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16EXT,  { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16EXT,  { 0, 0 } },
//    { vk::Format::eG16B16R162Plane444UnormEXT,               { 0, 0 } },
//    { vk::Format::eA4R4G4B4UnormPack16EXT,                   { 0, 0 } },
//    { vk::Format::eA4B4G4R4UnormPack16EXT,                   { 0, 0 } },
//    { vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR,   { 0, 0 } },
//    { vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR,   { 0, 0 } },
//    { vk::Format::eB16G16R16G16422UnormKHR,                  { 0, 0 } },
//    { vk::Format::eB8G8R8G8422UnormKHR,                      { 0, 0 } },
//    { vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR,   { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR,   { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16KHR,  { 0, 0 } },
//    { vk::Format::eG16B16G16R16422UnormKHR,                  { 0, 0 } },
//    { vk::Format::eG16B16R162Plane420UnormKHR,               { 0, 0 } },
//    { vk::Format::eG16B16R162Plane422UnormKHR,               { 0, 0 } },
//    { vk::Format::eG16B16R163Plane420UnormKHR,               { 0, 0 } },
//    { vk::Format::eG16B16R163Plane422UnormKHR,               { 0, 0 } },
//    { vk::Format::eG16B16R163Plane444UnormKHR,               { 0, 0 } },
//    { vk::Format::eG8B8G8R8422UnormKHR,                      { 0, 0 } },
//    { vk::Format::eG8B8R82Plane420UnormKHR,                  { 0, 0 } },
//    { vk::Format::eG8B8R82Plane422UnormKHR,                  { 0, 0 } },
//    { vk::Format::eG8B8R83Plane420UnormKHR,                  { 0, 0 } },
//    { vk::Format::eG8B8R83Plane422UnormKHR,                  { 0, 0 } },
//    { vk::Format::eG8B8R83Plane444UnormKHR,                  { 0, 0 } },
//    { vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16KHR,      { 0, 0 } },
//    { vk::Format::eR10X6G10X6Unorm2Pack16KHR,                { 0, 0 } },
//    { vk::Format::eR10X6UnormPack16KHR,                      { 0, 0 } },
//    { vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16KHR,      { 0, 0 } },
//    { vk::Format::eR12X4G12X4Unorm2Pack16KHR,                { 0, 0 } },
//    { vk::Format::eR12X4UnormPack16KHR,                      { 0, 0 } },
};

void Texture::Dispose::operator()(void* texture) {
    GetGfxDevice().DestroyTexture(texture);
}

Texture::Texture(VkImage image, VkImageView view, VkSampler sampler, VmaAllocation allocation) {
    impl.reset(GetGfxDevice().CreateTexture(image, view, sampler, allocation));
}

auto Texture::getImage() const -> vk::Image {
    return static_cast<VulkanTexture*>(impl.get())->image;
}

auto Texture::getSampler() const -> vk::Sampler {
    return static_cast<VulkanTexture*>(impl.get())->sampler;
}

auto Texture::getImageView() const -> vk::ImageView {
    return static_cast<VulkanTexture*>(impl.get())->imageView;
}

Texture2D::Texture2D(glm::u32 width, glm::u32 height, vk::Format format) : _width(width), _height(height) {
    impl.reset(GetGfxDevice().CreateTexture(width, height, format));
}

void Texture2D::setPixels(std::span<const glm::u8vec4> pixels) {
    auto stagingBuffer = GraphicsBuffer(GraphicsBuffer::Target::CopySrc, static_cast<int>(pixels.size_bytes()));
    stagingBuffer.setData(std::as_bytes(pixels), 0);

    const auto copy_barrier = vk::ImageMemoryBarrier{
        .dstAccessMask = vk::AccessFlagBits::eTransferWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eTransferDstOptimal,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = getImage(),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    const auto region = vk::BufferImageCopy{
        .imageSubresource = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .layerCount = 1
        },
        .imageExtent = {
            .width = _width,
            .height = _height,
            .depth = 1
        }
    };

    const auto use_barrier = vk::ImageMemoryBarrier{
        .srcAccessMask = vk::AccessFlagBits::eTransferWrite,
        .dstAccessMask = vk::AccessFlagBits::eShaderRead,
        .oldLayout = vk::ImageLayout::eTransferDstOptimal,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = getImage(),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    auto vk_stagingBuffer = static_cast<VulkanGraphicsBuffer*>(stagingBuffer.getNativeBufferPtr())->buffer;

    auto fence = Graphics::CreateGraphicsFence();
    auto pool = Graphics::CreateCommandPool();
    auto cmd = pool.allocate();
    (*cmd).begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    (*cmd).pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {copy_barrier});
    (*cmd).copyBufferToImage(vk_stagingBuffer, getImage(), vk::ImageLayout::eTransferDstOptimal, {region});
    (*cmd).pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {use_barrier});
    (*cmd).end();
    Graphics::ExecuteCommandBuffer(cmd, fence);
    Graphics::WaitOnGraphicsFence(fence);
    pool.free(cmd);
}

void RenderTexture::Dispose::operator()(void* texture) {
//    GetGfxDevice().DestroyTexture(texture);
}

RenderTexture::RenderTexture(const RenderTextureDescriptor &descriptor) : descriptor(descriptor) {

}