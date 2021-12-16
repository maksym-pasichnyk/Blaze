#include "VulkanSwapchain.hpp"
#include "VulkanGfxDevice.hpp"

#include <Texture.hpp>
#include <Graphics.hpp>
#include <CommandPool.hpp>
#include <CommandBuffer.hpp>

static auto SelectSurfaceExtent(vk::Extent2D extent, const vk::SurfaceCapabilitiesKHR &surface_capabilities) -> vk::Extent2D {
    if (surface_capabilities.currentExtent.width != std::numeric_limits<glm::u32>::max()) {
        return surface_capabilities.currentExtent;
    }

    const auto minExtent = surface_capabilities.minImageExtent;
    const auto maxExtent = surface_capabilities.maxImageExtent;

    return {
        std::clamp(extent.width, minExtent.width, maxExtent.width),
        std::clamp(extent.height, minExtent.height, maxExtent.height)
    };
}

static auto SelectSurfaceFormat(std::span<const vk::SurfaceFormatKHR> surface_formats, std::span<const vk::Format> request_formats, vk::ColorSpaceKHR request_color_space) -> vk::SurfaceFormatKHR {
    if (surface_formats.size() == 1) {
        if (surface_formats.front().format == vk::Format::eUndefined) {
            return vk::SurfaceFormatKHR {
                .format = request_formats.front(),
                .colorSpace = request_color_space
            };
        }
        return surface_formats.front();
    }

    for (auto&& request_format : request_formats) {
        for (auto&& surface_format : surface_formats) {
            if (surface_format.format == request_format && surface_format.colorSpace == request_color_space) {
                return surface_format;
            }
        }
    }
    return surface_formats.front();
}

static auto SelectPresentMode(std::span<const vk::PresentModeKHR> present_modes, std::span<const vk::PresentModeKHR> request_modes) -> vk::PresentModeKHR {
    for (auto request_mode : request_modes) {
        for (auto present_mode : present_modes) {
            if (request_mode == present_mode) {
                return request_mode;
            }
        }
    }
    return vk::PresentModeKHR::eFifo;
}

static auto FindSupportedFormat(vk::PhysicalDevice device, std::span<const vk::Format> formats, vk::FormatFeatureFlags flags) -> vk::Format {
    for (auto format : formats) {
        const auto properties = device.getFormatProperties(format);
        if ((properties.optimalTilingFeatures & flags) == flags) {
            return format;
        }
    }
    for (auto format : formats) {
        const auto properties = device.getFormatProperties(format);
        if ((properties.linearTilingFeatures & flags) == flags) {
            return format;
        }
    }
    return vk::Format::eUndefined;
}

static auto GetSupportedDepthFormat(vk::PhysicalDevice physicalDevice) -> vk::Format {
    static constexpr auto formats = std::array{
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
        vk::Format::eD32Sfloat
    };
    return FindSupportedFormat(
        physicalDevice,
        formats,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

VulkanSwapchain::VulkanSwapchain(VulkanGfxDevice& gfx)
    : _logicalDevice(gfx.getLogicalDevice())
    , _physicalDevice(gfx.getPhysicalDevice())
    , _surface(gfx.getSurface())
    , _presentQueue(gfx.getPresentQueue())
    , _graphicsQueue(gfx.getGraphicsQueue())
    , _presentFamily(gfx.getPresentFamily())
    , _graphicsFamily(gfx.getGraphicsFamily())
    , _depthFormat(GetSupportedDepthFormat(_physicalDevice))
{
    _createSwapchain();
    _createRenderPass();
    _createSyncObjects();
    _createFrameObjects();
}

VulkanSwapchain::~VulkanSwapchain() {
    for (uint32_t i = 0; i < _frameCount; i++) {
        _logicalDevice.destroyFramebuffer(_framebuffers[i], nullptr);

        _depthTextures[i] = {};
        _swapchainTextures[i] = {};

        _cmdPools[i].free(_cmdBuffers[i]);

        _logicalDevice.destroyFence(_fences[i], nullptr);
        _logicalDevice.destroySemaphore(_acquiredSemaphores[i], nullptr);
        _logicalDevice.destroySemaphore(_completeSemaphores[i], nullptr);
    }

    _logicalDevice.destroyRenderPass(_renderPass, nullptr);
    _logicalDevice.destroySwapchainKHR(_swapchain, nullptr);
}

void VulkanSwapchain::_createSwapchain() {
    const auto formats = _physicalDevice.getSurfaceFormatsKHR(_surface);
    const auto capabilities = _physicalDevice.getSurfaceCapabilitiesKHR(_surface);
    const auto presentModes = _physicalDevice.getSurfacePresentModesKHR(_surface);

    const auto requestFormats = std::array {
        vk::Format::eB8G8R8A8Unorm,
        vk::Format::eR8G8B8A8Unorm,
        vk::Format::eB8G8R8Unorm,
        vk::Format::eR8G8B8Unorm
    };

    const auto requestModes = std::array {
        vk::PresentModeKHR::eFifo
    };

    _surfaceExtent = SelectSurfaceExtent({0, 0}, capabilities);
    _surfaceFormat = SelectSurfaceFormat(formats, requestFormats, vk::ColorSpaceKHR::eSrgbNonlinear);
    _presentMode = SelectPresentMode(presentModes, requestModes);

    auto minImageCount = std::max(3u, capabilities.minImageCount);
    if (capabilities.maxImageCount != 0) {
        minImageCount = std::min(minImageCount, capabilities.maxImageCount);
    }

    const auto queueFamilyIndices = std::array{
        _graphicsFamily,
        _presentFamily
    };

    const auto flag = _graphicsFamily != _presentFamily;

    const auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR {
        .surface = _surface,
        .minImageCount = static_cast<uint32_t>(minImageCount),
        .imageFormat = _surfaceFormat.format,
        .imageColorSpace = _surfaceFormat.colorSpace,
        .imageExtent = _surfaceExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = flag ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = flag ? static_cast<uint32_t>(queueFamilyIndices.size()) : 0,
        .pQueueFamilyIndices = flag ? queueFamilyIndices.data() : nullptr,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = _presentMode,
        .clipped = true,
        .oldSwapchain = nullptr
    };

    _swapchain = _logicalDevice.createSwapchainKHR(swapchainCreateInfo, nullptr);
    _swapchainImages = _logicalDevice.getSwapchainImagesKHR(_swapchain);
    _frameCount = _swapchainImages.size();
}

void VulkanSwapchain::_createRenderPass() {
    const auto attachments = std::array {
        vk::AttachmentDescription{
            {},
            _surfaceFormat.format,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR
        },
        vk::AttachmentDescription{
            {},
            _depthFormat,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal
        }
    };

    const auto color_attachment = vk::AttachmentReference{
        0,
        vk::ImageLayout::eColorAttachmentOptimal
    };

    const auto depth_attachment = vk::AttachmentReference{
        1,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    };

    const auto subpass = vk::SubpassDescription{
        {},
        vk::PipelineBindPoint::eGraphics,
        0,
        nullptr,
        1,
        &color_attachment,
        nullptr,
        &depth_attachment,
        0,
        nullptr
    };

    const auto dependency = vk::SubpassDependency{
        VK_SUBPASS_EXTERNAL,
        0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {},
        vk::AccessFlagBits::eColorAttachmentWrite
    };

    const auto renderPassCreateInfo = vk::RenderPassCreateInfo{}
        .setAttachments(attachments)
        .setSubpasses(subpass)
        .setDependencies(dependency);

    _renderPass = _logicalDevice.createRenderPass(renderPassCreateInfo, nullptr);
}

void VulkanSwapchain::_createSyncObjects() {
    _fences.resize(_frameCount);
    _acquiredSemaphores.resize(_frameCount);
    _completeSemaphores.resize(_frameCount);

    for (uint32_t i = 0; i < _frameCount; i++) {
        _fences[i] = _logicalDevice.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
        _acquiredSemaphores[i] = _logicalDevice.createSemaphore({}, nullptr);
        _completeSemaphores[i] = _logicalDevice.createSemaphore({}, nullptr);
    }
}

void VulkanSwapchain::_createFrameObjects() {
    _cmdPools.resize(_frameCount);
    _cmdBuffers.resize(_frameCount);
    _framebuffers.resize(_frameCount);
    _depthTextures.resize(_frameCount);
    _swapchainTextures.resize(_frameCount);

    for (uint32_t i = 0; i < _frameCount; i++) {
        const auto swapchainImageViewCreateInfo = vk::ImageViewCreateInfo{
            .image = _swapchainImages[i],
            .viewType = vk::ImageViewType::e2D,
            .format = _surfaceFormat.format,
            .subresourceRange = {
                vk::ImageAspectFlagBits::eColor,
                0, 1, 0, 1
            }
        };

        const auto swapchainImageView = _logicalDevice.createImageView(swapchainImageViewCreateInfo, nullptr);

        _cmdPools[i] = Graphics::CreateCommandPool();
        _cmdBuffers[i] = _cmdPools[i].allocate();
        _depthTextures[i] = Texture2D(_surfaceExtent.width, _surfaceExtent.height, _depthFormat);
        _swapchainTextures[i] = Texture(_swapchainImages[i], swapchainImageView, {}, nullptr);

        const auto attachments = std::array {
            _swapchainTextures[i].getImageView(),
            _depthTextures[i].getImageView()
        };

        const auto framebufferCreateInfo = vk::FramebufferCreateInfo{
            .renderPass = _renderPass,
            .width = _surfaceExtent.width,
            .height = _surfaceExtent.height,
            .layers = 1
        }.setAttachments(attachments);

        _framebuffers[i] = _logicalDevice.createFramebuffer(framebufferCreateInfo, nullptr);
    }
}

auto VulkanSwapchain::begin(const glm::vec4& color, float depth, glm::u32 stencil) -> CommandBuffer {
    static constexpr auto timeout = std::numeric_limits<uint64_t>::max();
    const auto semaphore = _acquiredSemaphores[_frameIndex];

    _logicalDevice.waitForFences(1, &_fences[_frameIndex], true, timeout);
    _logicalDevice.resetFences(1, &_fences[_frameIndex]);
    _logicalDevice.acquireNextImageKHR(_swapchain, timeout, semaphore, nullptr, &_swapchainImageIndex);

    (*_cmdBuffers[_frameIndex]).begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

    const auto clearColors = std::array {
        vk::ClearValue{
            .color = {
                .float32 = std::array{
                    color.x,
                    color.y,
                    color.z,
                    color.w
                }
            }
        },
        vk::ClearValue{
            .depthStencil = vk::ClearDepthStencilValue{
                .depth = depth,
                .stencil = stencil
            }
        }
    };

    const auto renderArea = vk::Rect2D{
        .extent = _surfaceExtent
    };

    const auto beginInfo = vk::RenderPassBeginInfo{}
        .setRenderPass(_renderPass)
        .setFramebuffer(_framebuffers[_frameIndex])
        .setRenderArea(renderArea)
        .setClearValues(clearColors);

    (*_cmdBuffers[_frameIndex]).beginRenderPass(beginInfo, vk::SubpassContents::eInline);

    const auto viewport = vk::Viewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(_surfaceExtent.width),
        .height = static_cast<float>(_surfaceExtent.height),
        .minDepth = 0,
        .maxDepth = 1
    };

    const auto scissor = vk::Rect2D{
        vk::Offset2D{},
        _surfaceExtent
    };
    (*_cmdBuffers[_frameIndex]).setViewport(0, viewport);
    (*_cmdBuffers[_frameIndex]).setScissor(0, scissor);

    return _cmdBuffers[_frameIndex];
}

void VulkanSwapchain::present() {
    (*_cmdBuffers[_frameIndex]).endRenderPass();
    (*_cmdBuffers[_frameIndex]).end();

    const auto acquiredSemaphore = _acquiredSemaphores[_frameIndex];
    const auto completeSemaphore = _completeSemaphores[_frameIndex];

    const auto stages = std::array{
        vk::PipelineStageFlags{vk::PipelineStageFlagBits::eColorAttachmentOutput}
    };

    const auto cmdBuffers = std::array{
        *_cmdBuffers[_frameIndex]
    };

    const auto submitInfo = vk::SubmitInfo{}
        .setWaitDstStageMask(stages)
        .setCommandBuffers(cmdBuffers)
        .setWaitSemaphores(acquiredSemaphore)
        .setSignalSemaphores(completeSemaphore);

    _graphicsQueue.submit(1, &submitInfo, _fences[_frameIndex]);

    const auto presentInfo = vk::PresentInfoKHR{}
        .setWaitSemaphores(completeSemaphore)
        .setSwapchains(_swapchain)
        .setImageIndices(_swapchainImageIndex);

    _presentQueue.presentKHR(presentInfo);

    _frameIndex = (_frameIndex + 1) % _frameCount;
}