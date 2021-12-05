#include "Blaze.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include "Timestamp.hpp"
#include "UserInterface.hpp"
#include "BlazeInstance.hpp"
#include "RenderPipeline.hpp"

namespace {
    Timestamp s_Timestamp;

    Display        s_Display;
    Input          s_Input;
    BlazeInstance  s_Context;
    RenderPipeline s_Graphics;
    UserInterface  s_Overlay;
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

auto Blaze::GetInput() -> Input* {
    return &s_Input;
}

auto Blaze::GetDisplay() -> Display* {
    return &s_Display;
}

auto Blaze::GetGraphics() -> RenderPipeline* {
    return &s_Graphics;
}

auto Blaze::GetLogicalDevice() -> vk::Device {
    return s_Context.getLogicalDevice();
}

auto Blaze::GetPhysicalDevice() -> vk::PhysicalDevice {
    return s_Context.getPhysicalDevice();
}

auto Blaze::GetMemoryResource() -> VmaAllocator {
    return s_Context.getMemoryResource();
}

auto Blaze::GetPresentQueue() -> vk::Queue {
    return s_Context.getPresentQueue();
}

auto Blaze::GetGraphicsQueue() -> vk::Queue {
    return s_Context.getGraphicsQueue();
}

auto Blaze::GetPresentFamily() -> uint32_t {
    return s_Context.getPresentFamily();
}

auto Blaze::GetGraphicsFamily() -> uint32_t {
    return s_Context.getGraphicsFamily();
}

auto Blaze::GetSurface() -> vk::SurfaceKHR {
    return s_Context.getSurface();
}

auto Blaze::GetSupportedDepthFormat() -> vk::Format {
    static constexpr auto formats = std::array{
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
        vk::Format::eD32Sfloat
    };
    return FindSupportedFormat(
        GetPhysicalDevice(),
        formats,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

auto Blaze::GetFrameCount() -> size_t {
    return s_Graphics.getFrameCount();
}

auto Blaze::GetSurfaceExtent() -> vk::Extent2D {
    return s_Graphics.getSurfaceExtent();
}

auto Blaze::GetDeltaTime() -> float {
    return static_cast<float>(s_Timestamp.seconds());
}

void Blaze::Start(const std::string& title, int width, int height, bool resizable, App& app) {
    s_Display.init(title, width, height, resizable);
    //    s_Input.init();
    s_Context.init(s_Display);
    s_Graphics.init();

    s_Overlay.init();

    const auto extent = s_Graphics.getSurfaceExtent();
    const auto viewport = vk::Viewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0,
        .maxDepth = 1
    };

    const auto scissor = vk::Rect2D{
        .extent = extent
    };

    app.Init();

    using Clock = std::chrono::high_resolution_clock;

    auto lastTime = Clock::now();
    while (!s_Display.shouldClose()) {
        s_Display.pollEvents();

        const auto currentTime = Clock::now();
        s_Timestamp = Timestamp{currentTime - lastTime};
        lastTime = currentTime;

        app.Update();

        s_Overlay.setMousePosition(s_Display.getMousePosition());
        s_Overlay.setDisplayScale(s_Display.getScale());
        s_Overlay.setDisplaySize(s_Display.getSize());
        s_Overlay.setDeltaTime(s_Timestamp);

        for (int i = 0; i < 5; i++) {
            s_Overlay.setMousePressed(i, s_Display.getMousePressed(i));
        }

        s_Overlay.begin();
        app.Overlay();
        s_Overlay.end();

        auto cmd = s_Graphics.begin();
        (*cmd).setViewport(0, viewport);
        (*cmd).setScissor(0, scissor);

        app.Render(cmd);

        s_Overlay.draw(*cmd);
        s_Graphics.present();
    }

    GetLogicalDevice().waitIdle();

    app.Destroy();

    s_Overlay = {};
    s_Graphics = {};
    s_Context = {};
    s_Input = {};
    s_Display = {};
}

extern void Start(int argc, char* argv[]);

auto main(int argc, char* argv[]) -> int {
    Start(argc, argv);
    return 0;
}