#include "VulkanTexture.hpp"
#include "VulkanGfxDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanGraphicsBuffer.hpp"

#include <Display.hpp>
#include <CommandBuffer.hpp>
#include <GraphicsBuffer.hpp>

#include <spdlog/spdlog.h>

namespace vk {
    VULKAN_HPP_STORAGE_API DispatchLoaderDynamic defaultDispatchLoaderDynamic;
}

static VKAPI_ATTR auto VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData
) -> VkBool32 {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        spdlog::info("{}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        spdlog::info("{}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        spdlog::warn("{}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        spdlog::error("{}", pCallbackData->pMessage);
    } else {
        spdlog::info("{}", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

static auto FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> std::optional<std::pair<uint32_t, uint32_t>> {
    const auto properties = device.getQueueFamilyProperties();

    uint32_t graphics_family = -1;
    uint32_t present_family = -1;
    for (uint32_t i = 0; i < uint32_t(properties.size()); i++) {
        if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphics_family = i;
        }

        if (device.getSurfaceSupportKHR(i, surface)) {
            present_family = i;
        }

        if ((graphics_family != -1) && (present_family != -1)) {
            return std::pair{ graphics_family, present_family };
        }
    }
    return std::nullopt;
}

static constexpr auto GetBufferUsageFromTarget(GraphicsBuffer::Target target) -> vk::BufferUsageFlags {
    using Type = std::underlying_type_t<GraphicsBuffer::Target>;

    auto flags = vk::BufferUsageFlags{};
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::Vertex)) {
        flags |= vk::BufferUsageFlagBits::eVertexBuffer;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::Index)) {
        flags |= vk::BufferUsageFlagBits::eIndexBuffer;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopySrc)) {
        flags |= vk::BufferUsageFlagBits::eTransferSrc;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopyDst)) {
        flags |= vk::BufferUsageFlagBits::eTransferDst;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::Constant)) {
        flags |= vk::BufferUsageFlagBits::eUniformBuffer;
    }
    return flags;
}

static constexpr auto GetMemoryUsageFromTarget(GraphicsBuffer::Target target) -> VmaMemoryUsage {
    using Type = std::underlying_type_t<GraphicsBuffer::Target>;

    auto flags = VMA_MEMORY_USAGE_CPU_TO_GPU;
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopySrc)) {
        flags = VMA_MEMORY_USAGE_CPU_ONLY;
    }
    if (static_cast<Type>(target) & static_cast<Type>(GraphicsBuffer::Target::CopyDst)) {
        flags = VMA_MEMORY_USAGE_GPU_ONLY;
    }
    return flags;
}

static constexpr auto IsDepthFormat(vk::Format format) -> bool {
    switch (format) {
        case vk::Format::eD16Unorm:
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD32Sfloat:
        case vk::Format::eS8Uint:
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD32SfloatS8Uint:
            return true;
        default:
            return false;
    }
}

static constexpr auto GetImageUsageFromFormat(vk::Format format) -> vk::ImageUsageFlags {
    switch (format) {
        case vk::Format::eD16Unorm:
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD32Sfloat:
        case vk::Format::eS8Uint:
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD32SfloatS8Uint:
            return vk::ImageUsageFlagBits::eDepthStencilAttachment;
        default:
            return vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    }
}

static constexpr auto GetImageAspectFromFormat(vk::Format format) -> vk::ImageAspectFlags {
    switch (format) {
        case vk::Format::eD16Unorm:
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD32Sfloat:
            return vk::ImageAspectFlagBits::eDepth;
        case vk::Format::eS8Uint:
            return vk::ImageAspectFlagBits::eStencil;
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD32SfloatS8Uint:
            return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
        default:
            return vk::ImageAspectFlagBits::eColor;
    }
}

VulkanGfxDevice::VulkanGfxDevice(Display& display) {
    _createInstance(display);
    _createSurface(display);
    _selectPhysicalDevice();
    _createLogicalDevice();
    _createMemoryResource();
}

VulkanGfxDevice::~VulkanGfxDevice() {
    vmaDestroyAllocator(_allocator);

    _logicalDevice.destroy();

    _instance.destroyDebugUtilsMessengerEXT(_debugUtils);
    _instance.destroySurfaceKHR(_surface);
    _instance.destroy();
}

void VulkanGfxDevice::_createInstance(Display& display) {
    VULKAN_HPP_DEFAULT_DISPATCHER.init(dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

    auto extensions = display.getInstanceExtensions();
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    const auto enabledLayers = std::array{
        "VK_LAYER_KHRONOS_validation"
    };

    auto appInfo = vk::ApplicationInfo{
        .pApplicationName = nullptr,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Craft Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_2
    };

    const auto instanceCreateInfo = vk::InstanceCreateInfo {
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = std::size(enabledLayers),
        .ppEnabledLayerNames = std::data(enabledLayers),
        .enabledExtensionCount = uint32_t(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };
    _instance = vk::createInstance(instanceCreateInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);

    const auto debugCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                       vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                       vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        .pfnUserCallback = DebugCallback
    };

    _debugUtils = _instance.createDebugUtilsMessengerEXT(debugCreateInfo);
}

void VulkanGfxDevice::_createSurface(Display& display) {
    _surface = display.createSurface(_instance);
}

void VulkanGfxDevice::_selectPhysicalDevice() {
    for (auto device : _instance.enumeratePhysicalDevices()) {
        const auto families = FindQueueFamilies(device, _surface);
        if (!families) {
            continue;
        }

        uint32_t surfaceFormatCount = 0;
        device.getSurfaceFormatsKHR(_surface, &surfaceFormatCount, nullptr);
        if (surfaceFormatCount == 0) {
            continue;
        }

        uint32_t presentModeCount = 0;
        device.getSurfacePresentModesKHR(_surface, &presentModeCount, nullptr);
        if (presentModeCount == 0) {
            continue;
        }

        _physicalDevice = device;
        _graphicsFamily = families->first;
        _presentFamily = families->second;
        return;
    }
}

void VulkanGfxDevice::_createLogicalDevice() {
    const auto queuePriority = 1.0f;

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};

    const auto graphicsQueueCreateInfo = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _graphicsFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };
    queueCreateInfos.emplace_back(graphicsQueueCreateInfo);

    if (_graphicsFamily != _presentFamily) {
        const auto presentQueueCreateInfo = vk::DeviceQueueCreateInfo {
            .queueFamilyIndex = _presentFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };

        queueCreateInfos.emplace_back(presentQueueCreateInfo);
    }

    static constexpr auto deviceExtensions = std::array{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
#ifdef __APPLE__
        VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
#endif
//        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    const auto features = vk::PhysicalDeviceFeatures{
        .fillModeNonSolid = VK_TRUE,
        .samplerAnisotropy = VK_TRUE
    };

//        auto dynamicRenderingFeaturesKHR = vk::PhysicalDeviceDynamicRenderingFeaturesKHR{
//            .dynamicRendering = true
//        };

//        const auto physicalDeviceFeatures2 = vk::PhysicalDeviceFeatures2{
//            .pNext = &dynamicRenderingFeaturesKHR,
//            .features = features
//        };

    const auto deviceCreateInfo = vk::DeviceCreateInfo {
//            .pNext = &physicalDeviceFeatures2,
        .queueCreateInfoCount = uint32_t(std::size(queueCreateInfos)),
        .pQueueCreateInfos = std::data(queueCreateInfos),
//			.enabledLayerCount = std::size(enabledLayers),
//			.ppEnabledLayerNames = std::data(enabledLayers),
        .enabledExtensionCount = std::size(deviceExtensions),
        .ppEnabledExtensionNames = std::data(deviceExtensions),
        .pEnabledFeatures = &features
    };

    _logicalDevice = _physicalDevice.createDevice(deviceCreateInfo, nullptr);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_logicalDevice);

    _presentQueue = _logicalDevice.getQueue(_presentFamily, 0);
    _graphicsQueue = _logicalDevice.getQueue(_graphicsFamily, 0);
}

void VulkanGfxDevice::_createMemoryResource() {
    const auto functions = VmaVulkanFunctions{
        .vkGetPhysicalDeviceProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkAllocateMemory,
        .vkFreeMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkFreeMemory,
        .vkMapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkMapMemory,
        .vkUnmapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkUnmapMemory,
        .vkFlushMappedMemoryRanges = VULKAN_HPP_DEFAULT_DISPATCHER.vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges = VULKAN_HPP_DEFAULT_DISPATCHER.vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory,
        .vkBindImageMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory,
        .vkGetBufferMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements,
        .vkCreateBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateBuffer,
        .vkDestroyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyBuffer,
        .vkCreateImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateImage,
        .vkDestroyImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyImage,
        .vkCmdCopyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdCopyBuffer,
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
        /// Fetch "vkGetBufferMemoryRequirements2" on Vulkan >= 1.1, fetch "vkGetBufferMemoryRequirements2KHR" when using VK_KHR_dedicated_allocation extension.
        .vkGetBufferMemoryRequirements2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements2KHR,
        /// Fetch "vkGetImageMemoryRequirements 2" on Vulkan >= 1.1, fetch "vkGetImageMemoryRequirements2KHR" when using VK_KHR_dedicated_allocation extension.
        .vkGetImageMemoryRequirements2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements2KHR,
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
        /// Fetch "vkBindBufferMemory2" on Vulkan >= 1.1, fetch "vkBindBufferMemory2KHR" when using VK_KHR_bind_memory2 extension.
        .vkBindBufferMemory2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory2KHR,
        /// Fetch "vkBindImageMemory2" on Vulkan >= 1.1, fetch "vkBindImageMemory2KHR" when using VK_KHR_bind_memory2 extension.
        .vkBindImageMemory2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory2KHR,
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
        .vkGetPhysicalDeviceMemoryProperties2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties2KHR,
#endif
    };

    const auto allocatorCreateInfo = VmaAllocatorCreateInfo{
        .physicalDevice = _physicalDevice,
        .device = _logicalDevice,
        .pVulkanFunctions = &functions,
        .instance = _instance,
        .vulkanApiVersion = VK_API_VERSION_1_2
    };

    vmaCreateAllocator(&allocatorCreateInfo, &_allocator);
}

void VulkanGfxDevice::WaitIdle() {
    _logicalDevice.waitIdle();
}

auto VulkanGfxDevice::CreateGPUFence() -> void* {
    return static_cast<VkFence>(_logicalDevice.createFence({}));
}

void VulkanGfxDevice::DestroyGPUFence(void* fence) {
    _logicalDevice.destroyFence(static_cast<VkFence>(fence));
}

void VulkanGfxDevice::WaitOnGPUFence(void* fence) {
    const auto vk_fence = static_cast<vk::Fence>(static_cast<VkFence>(fence));
    _logicalDevice.waitForFences(vk_fence, false, std::numeric_limits<uint64_t>::max());
}

void VulkanGfxDevice::ExecuteCommandBuffer(const CommandBuffer &cmd, void* fence) {
    const auto vk_fence = static_cast<vk::Fence>(static_cast<VkFence>(fence));

    const auto commandBuffers = std::array{ *cmd };
    const auto submitInfo = vk::SubmitInfo{}
        .setCommandBuffers(commandBuffers);

    _graphicsQueue.submit(1, &submitInfo, vk_fence);
}

auto VulkanGfxDevice::CreateBuffer(GraphicsBuffer::Target target, int size) -> void* {
    const auto bufferCreateInfo = static_cast<VkBufferCreateInfo>(vk::BufferCreateInfo {
        .size = static_cast<vk::DeviceSize>(size),
        .usage = GetBufferUsageFromTarget(target)
    });

    const auto allocCreateInfo = VmaAllocationCreateInfo {
        .usage = GetMemoryUsageFromTarget(target)
    };

    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocationInfo;

    vmaCreateBuffer(
        _allocator,
        &bufferCreateInfo,
        &allocCreateInfo,
        &buffer,
        &allocation,
        &allocationInfo
    );

    return new VulkanGraphicsBuffer {
        .buffer = buffer,
        .allocation = allocation,
        .allocationInfo = allocationInfo
    };
}

void VulkanGfxDevice::DestroyBuffer(void* buffer) {
    auto vk_buffer = static_cast<VulkanGraphicsBuffer*>(buffer);
    vmaDestroyBuffer(_allocator, vk_buffer->buffer, vk_buffer->allocation);
    delete vk_buffer;
}

void VulkanGfxDevice::UpdateBuffer(void* buffer, std::span<const std::byte> bytes, size_t offset) {
    auto vk_buffer = static_cast<VulkanGraphicsBuffer*>(buffer);
    void* ptr = nullptr;
    vmaMapMemory(_allocator, vk_buffer->allocation, &ptr);
    std::copy(bytes.begin(), bytes.end(), static_cast<std::byte*>(ptr) + offset);
    vmaUnmapMemory(_allocator, vk_buffer->allocation);
}

auto VulkanGfxDevice::CreateCommandPool() -> void* {
    const auto createInfo = vk::CommandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = _graphicsFamily
    };
    return _logicalDevice.createCommandPool(createInfo, nullptr);
}
void VulkanGfxDevice::DestroyCommandPool(void* pool) {
    auto vk_pool = static_cast<VkCommandPool>(pool);
    _logicalDevice.destroyCommandPool(vk_pool, nullptr);
}

auto VulkanGfxDevice::AllocateCommandBuffer(void *pool) -> void* {
    auto vk_pool = static_cast<VkCommandPool>(pool);

    const auto allocateInfo = vk::CommandBufferAllocateInfo{
        .commandPool = vk_pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    return _logicalDevice.allocateCommandBuffers(allocateInfo)[0];
}

void VulkanGfxDevice::FreeCommandBuffer(void* pool, void* cmd) {
    const auto vk_pool = static_cast<VkCommandPool>(pool);
    const auto vk_cmd = static_cast<VkCommandBuffer>(cmd);
    _logicalDevice.freeCommandBuffers(vk_pool, { vk_cmd });
}

auto VulkanGfxDevice::CreateTexture(glm::u32 width, glm::u32 height, vk::Format format) -> void* {
    const auto imageCreateInfo = static_cast<VkImageCreateInfo>(vk::ImageCreateInfo{
        .imageType = vk::ImageType::e2D,
        .format = format,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .usage = GetImageUsageFromFormat(format)
    });

    VkImage image;
    VmaAllocation allocation;

    const auto allocationCreateInfo = VmaAllocationCreateInfo{
        .usage = VMA_MEMORY_USAGE_GPU_ONLY
    };
    vmaCreateImage(_allocator, &imageCreateInfo, &allocationCreateInfo, &image, &allocation, nullptr);

    const auto imageViewCreateInfo = vk::ImageViewCreateInfo{
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange = {
            .aspectMask = GetImageAspectFromFormat(format),
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    const auto imageView = _logicalDevice.createImageView(imageViewCreateInfo);

    const auto sampler = IsDepthFormat(format) ? nullptr : _logicalDevice.createSampler(vk::SamplerCreateInfo{
//        .magFilter = vk::Filter::eLinear,
//        .minFilter = vk::Filter::eLinear,
//        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat,
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .maxAnisotropy = 1.0f,
        .minLod = -1000,
        .maxLod = 1000
    });

    return new VulkanTexture{
        .image = image,
        .sampler = sampler,
        .imageView = imageView,
        .allocation = allocation
    };
}

auto VulkanGfxDevice::CreateTexture(VkImage image, VkImageView imageView, VkSampler sampler, VmaAllocation allocation) -> void* {
    return new VulkanTexture{
        .image = image,
        .sampler = sampler,
        .imageView = imageView,
        .allocation = allocation
    };
}

void VulkanGfxDevice::DestroyTexture(void* texture) {
    const auto vk_texture = static_cast<VulkanTexture*>(texture);

    if (vk_texture->allocation) {
        vmaDestroyImage(_allocator, vk_texture->image, vk_texture->allocation);
    }
    if (vk_texture->sampler) {
        _logicalDevice.destroySampler(vk_texture->sampler, nullptr);
    }
    _logicalDevice.destroyImageView(vk_texture->imageView, nullptr);
}
