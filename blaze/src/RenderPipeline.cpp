#include "RenderPipeline.hpp"
#include "Blaze.hpp"

static auto GetImageCountFromPresentMode(vk::PresentModeKHR present_mode) -> uint32_t {
    switch (present_mode) {
        case vk::PresentModeKHR::eImmediate:
            return 1;
        case vk::PresentModeKHR::eMailbox:
            return 3;
        case vk::PresentModeKHR::eFifo:
        case vk::PresentModeKHR::eFifoRelaxed:
            return 2;
//		case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
//			break;
//		case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
//			break;
        default:
            return 1;
    }
}

static auto SelectSurfaceExtent(vk::Extent2D extent, const vk::SurfaceCapabilitiesKHR &surface_capabilities) -> vk::Extent2D {
    if (surface_capabilities.currentExtent.width != UINT32_MAX) {
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

struct RenderPipeline::Impl {
    uint32_t _frameIndex = 0;
    uint32_t _frameCount = 0;
    uint32_t _semaphoreIndex = 0;

    vk::Format _depthFormat;
    vk::RenderPass _renderPass;

    vk::Extent2D _surfaceExtent;
    vk::PresentModeKHR _presentMode;
    vk::SurfaceFormatKHR _surfaceFormat;

    vk::SwapchainKHR _swapchain;

// frame objects

    std::vector<vk::Fence> _fences;
    std::vector<vk::Semaphore> _acquiredSemaphores;
    std::vector<vk::Semaphore> _completeSemaphores;

    std::vector<vk::Image> _swapchainImages;
    std::vector<Texture> _swapchainTextures;
    std::vector<Texture> _depthTextures;
    std::vector<vk::Framebuffer> _framebuffers;

    std::vector<CommandPool> _commandPools;
    std::vector<CommandBuffer> _commandBuffers;

    Impl() {
        _createSwapchain();
        _createRenderPass();
        _createSyncObjects();
        _createFrameObjects();
    }

    ~Impl() {
        for (uint32_t i = 0; i < _frameCount; i++) {
            Blaze::GetLogicalDevice().destroyFramebuffer(_framebuffers[i], nullptr);

            _depthTextures[i] = {};
            _swapchainTextures[i] = {};

            _commandPools[i].free(_commandBuffers[i]);
            _commandPools[i] = {};

            Blaze::GetLogicalDevice().destroyFence(_fences[i], nullptr);
            Blaze::GetLogicalDevice().destroySemaphore(_acquiredSemaphores[i], nullptr);
            Blaze::GetLogicalDevice().destroySemaphore(_completeSemaphores[i], nullptr);
        }

        Blaze::GetLogicalDevice().destroyRenderPass(_renderPass, nullptr);
        Blaze::GetLogicalDevice().destroySwapchainKHR(_swapchain, nullptr);
    }

    void _createSwapchain() {
        const auto capabilities = Blaze::GetPhysicalDevice().getSurfaceCapabilitiesKHR(Blaze::GetSurface());
        const auto surfaceFormats = Blaze::GetPhysicalDevice().getSurfaceFormatsKHR(Blaze::GetSurface());
        const auto presentModes = Blaze::GetPhysicalDevice().getSurfacePresentModesKHR(Blaze::GetSurface());

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
        _surfaceFormat = SelectSurfaceFormat(surfaceFormats, requestFormats, vk::ColorSpaceKHR::eSrgbNonlinear);
        _presentMode = SelectPresentMode(presentModes, requestModes);

        auto minImageCount = GetImageCountFromPresentMode(_presentMode);
        if (minImageCount < capabilities.minImageCount) {
            minImageCount = capabilities.minImageCount;
        } else if (capabilities.maxImageCount != 0 && minImageCount > capabilities.maxImageCount) {
            minImageCount = capabilities.maxImageCount;
        }

        const auto queueFamilyIndices = std::array{
            Blaze::GetGraphicsFamily(),
            Blaze::GetPresentFamily()
        };

        const auto flag = Blaze::GetGraphicsFamily() != Blaze::GetPresentFamily();

        const auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR {
            .surface = Blaze::GetSurface(),
            .minImageCount = static_cast<uint32_t>(minImageCount),
            .imageFormat = _surfaceFormat.format,
            .imageColorSpace = _surfaceFormat.colorSpace,
            .imageExtent = _surfaceExtent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = flag ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
            .queueFamilyIndexCount = flag ? uint32_t(queueFamilyIndices.size()) : 0,
            .pQueueFamilyIndices = flag ? queueFamilyIndices.data() : nullptr,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode = _presentMode,
            .clipped = true,
            .oldSwapchain = nullptr
        };

        _swapchain = Blaze::GetLogicalDevice().createSwapchainKHR(swapchainCreateInfo, nullptr);
        _swapchainImages = Blaze::GetLogicalDevice().getSwapchainImagesKHR(_swapchain);
        _frameCount = _swapchainImages.size();
    }

    void _createRenderPass() {
        _depthFormat = Blaze::GetSupportedDepthFormat();

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

        const auto renderPassCreateInfo = vk::RenderPassCreateInfo{
            .attachmentCount = attachments.size(),
            .pAttachments = attachments.data(),
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency,
        };
        _renderPass = Blaze::GetLogicalDevice().createRenderPass(renderPassCreateInfo, nullptr);
    }

    void _createSyncObjects() {
        _fences.resize(_frameCount);
        _acquiredSemaphores.resize(_frameCount);
        _completeSemaphores.resize(_frameCount);

        for (uint32_t i = 0; i < _frameCount; i++) {
            _fences[i] = Blaze::GetLogicalDevice().createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
            _acquiredSemaphores[i] = Blaze::GetLogicalDevice().createSemaphore({}, nullptr);
            _completeSemaphores[i] = Blaze::GetLogicalDevice().createSemaphore({}, nullptr);
        }
    }

    void _createFrameObjects() {
        _commandPools.resize(_frameCount);
        _commandBuffers.resize(_frameCount);
        _framebuffers.resize(_frameCount);
        _depthTextures.resize(_frameCount);
        _swapchainTextures.resize(_frameCount);
    //    _swapchainImageViews.resize(_frameCount);

        for (uint32_t i = 0; i < _frameCount; i++) {
            _depthTextures[i] = Texture::CreateDepthTexture(_depthFormat, _surfaceExtent);

            const auto swapchainImageViewCreateInfo = vk::ImageViewCreateInfo{
                .image = _swapchainImages[i],
                .viewType = vk::ImageViewType::e2D,
                .format = _surfaceFormat.format,
                .subresourceRange = {
                    vk::ImageAspectFlagBits::eColor,
                    0, 1, 0, 1
                }
            };

            const auto swapchainImageView = Blaze::GetLogicalDevice().createImageView(swapchainImageViewCreateInfo, nullptr);

            _swapchainTextures[i] = Texture(_swapchainImages[i], swapchainImageView, {}, nullptr);

            const auto attachments = std::array {
                _swapchainTextures[i].getImageView(),
                _depthTextures[i].getImageView()
            };

            const auto framebufferCreateInfo = vk::FramebufferCreateInfo{
                .renderPass = _renderPass,
                .attachmentCount = attachments.size(),
                .pAttachments = attachments.data(),
                .width = _surfaceExtent.width,
                .height = _surfaceExtent.height,
                .layers = 1
            };

            _framebuffers[i] = Blaze::GetLogicalDevice().createFramebuffer(framebufferCreateInfo, nullptr);
            _commandPools[i] = CommandPool(Blaze::GetGraphicsFamily());
            _commandBuffers[i] = _commandPools[i].allocate(vk::CommandBufferLevel::ePrimary);
        }
    }

    auto begin() -> CommandBuffer {
        static constexpr auto timeout = std::numeric_limits<uint64_t>::max();
        const auto semaphore = _acquiredSemaphores[_semaphoreIndex];

        Blaze::GetLogicalDevice().acquireNextImageKHR(_swapchain, timeout, semaphore, nullptr, &_frameIndex);
        Blaze::GetLogicalDevice().waitForFences(1, &_fences[_frameIndex], true, timeout);
        Blaze::GetLogicalDevice().resetFences(1, &_fences[_frameIndex]);

        (*_commandBuffers[_frameIndex]).begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

        const auto clearColors = std::array {
            vk::ClearValue{.color = {.float32 = std::array{1.0f, 0.0f, 0.0f, 1.0f}}},
            vk::ClearValue{.depthStencil = vk::ClearDepthStencilValue{1.0f, 0}}
        };

        const auto beginInfo = vk::RenderPassBeginInfo {
            .renderPass = _renderPass,
            .framebuffer = _framebuffers[_frameIndex],
            .renderArea = {
                .extent = _surfaceExtent
            },
            .clearValueCount = clearColors.size(),
            .pClearValues = clearColors.data()
        };

        (*_commandBuffers[_frameIndex]).beginRenderPass(beginInfo, vk::SubpassContents::eInline);
        return _commandBuffers[_frameIndex];
    }

    void present() {
        (*_commandBuffers[_frameIndex]).endRenderPass();
        (*_commandBuffers[_frameIndex]).end();

        const auto acquiredSemaphore = _acquiredSemaphores[_semaphoreIndex];
        const auto completeSemaphore = _completeSemaphores[_semaphoreIndex];

        const auto stages = std::array{
            vk::PipelineStageFlags{vk::PipelineStageFlagBits::eColorAttachmentOutput}
        };

        const auto commandBuffers = std::array{
            *_commandBuffers[_frameIndex]
        };

        const auto submitInfo = vk::SubmitInfo{}
            .setWaitDstStageMask(stages)
            .setCommandBuffers(commandBuffers)
            .setWaitSemaphores(acquiredSemaphore)
            .setSignalSemaphores(completeSemaphore);

        Blaze::GetGraphicsQueue().submit(1, &submitInfo, _fences[_frameIndex]);

        const auto presentInfo = vk::PresentInfoKHR{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &completeSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &_swapchain,
            .pImageIndices = &_frameIndex
        };
        Blaze::GetPresentQueue().presentKHR(presentInfo);
    //	Blaze::GetPresentQueue().waitIdle();

        _semaphoreIndex = (_semaphoreIndex + 1) % _frameCount;
    }
};

void RenderPipeline::init() {
    impl = blaze::make_internal<Impl>();
}

auto RenderPipeline::begin() -> CommandBuffer {
    return impl->begin();
}

void RenderPipeline::present() {
    impl->present();
}

auto RenderPipeline::getRenderPass() const -> vk::RenderPass {
    return impl->_renderPass;
}

auto RenderPipeline::getFrameCount() const -> size_t {
    return impl->_frameCount;
}

auto RenderPipeline::getSurfaceExtent() const -> vk::Extent2D {
    return impl->_surfaceExtent;
}