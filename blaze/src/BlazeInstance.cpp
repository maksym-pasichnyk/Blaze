#include "Blaze.hpp"
#include "BlazeInstance.hpp"
#include "Display.hpp"
#include "CommandPool.hpp"

#include <iostream>
#include <optional>
#include <vulkan/vulkan_beta.h>

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
        std::cout << pCallbackData->pMessage << std::endl;
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        std::cout << pCallbackData->pMessage << std::endl;
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cout << pCallbackData->pMessage << std::endl;
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cerr << pCallbackData->pMessage << std::endl;
    } else {
        std::cout << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}

static auto _findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> std::optional<std::pair<uint32_t, uint32_t>> {
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
            return std::pair{
                graphics_family,
                present_family
            };
        }
    }
    return std::nullopt;
}

struct BlazeInstance::Impl {
    vk::DynamicLoader dl;

    vk::Instance _instance;
    vk::Device _logicalDevice;
    vk::PhysicalDevice _physicalDevice;

    VmaAllocator _allocator;

    vk::SurfaceKHR _surface;
    vk::DebugUtilsMessengerEXT _debugUtils;

    uint32_t _graphicsFamily;
    uint32_t _presentFamily;

    vk::Queue _presentQueue;
    vk::Queue _graphicsQueue;

    Impl(Display& display) {
        _createInstance(display);
        _createSurface(display);
        _selectPhysicalDevice();
        _createLogicalDevice();
        _createMemoryResource();
    }

    ~Impl() {
        _logicalDevice.waitIdle();

        vmaDestroyAllocator(_allocator);

        _logicalDevice.destroy(nullptr);

        _instance.destroyDebugUtilsMessengerEXT(_debugUtils);
        _instance.destroySurfaceKHR(_surface, nullptr);
        _instance.destroy(nullptr);
    }

    void _createInstance(Display& display) {
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

    void _createSurface(Display& display) {
        _surface = display.createSurface(_instance);
    }

    void _selectPhysicalDevice() {
        for (auto device : _instance.enumeratePhysicalDevices()) {
            const auto families = _findQueueFamilies(device, _surface);
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

    void _createLogicalDevice() {
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
//            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
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

    void _createMemoryResource() {
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
};

void BlazeInstance::init(Display& display) {
    impl = blaze::make_internal<Impl>(display);
}

auto BlazeInstance::getLogicalDevice() const -> vk::Device {
    return impl->_logicalDevice;
}
auto BlazeInstance::getPhysicalDevice() const -> vk::PhysicalDevice {
    return impl->_physicalDevice;
}
auto BlazeInstance::getMemoryResource() const -> VmaAllocator {
    return impl->_allocator;
}
auto BlazeInstance::getPresentQueue() const -> vk::Queue {
    return impl->_presentQueue;
}
auto BlazeInstance::getGraphicsQueue() const -> vk::Queue {
    return impl->_graphicsQueue;
}
auto BlazeInstance::getPresentFamily() const -> uint32_t {
    return impl->_presentFamily;
}
auto BlazeInstance::getGraphicsFamily() const -> uint32_t {
    return impl->_graphicsFamily;
}
auto BlazeInstance::getSurface() const -> vk::SurfaceKHR {
    return impl->_surface;
}