#include "Time.hpp"
#include "Blaze.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include "UserInterface.hpp"
#include "GraphicsBuffer.hpp"

#include <VulkanGfxDevice.hpp>
#include <VulkanSwapchain.hpp>

struct VulkanRenderer {
    VulkanSwapchain swapchain;

    explicit VulkanRenderer(VulkanGfxDevice& device) : swapchain(device) {
    }
};

static std::unique_ptr<Display> display;
static std::unique_ptr<VulkanGfxDevice> device;
static std::unique_ptr<VulkanRenderer> renderer;
static std::unique_ptr<Input> input;
static std::unique_ptr<UserInterface> ui;

auto GetDisplay() -> Display& {
    return *display;
}

auto GetGfxDevice() -> VulkanGfxDevice& {
    return *device;
}

auto GetInputDevice() -> Input& {
    return *input;
}

auto Blaze::GetSwapchainRenderPass() -> vk::RenderPass {
    return renderer->swapchain.getRenderPass();
}

auto Blaze::GetLogicalDevice() -> vk::Device {
    return GetGfxDevice().getLogicalDevice();
}

auto Blaze::GetMemoryResource() -> VmaAllocator {
    return GetGfxDevice().getMemoryResource();
}

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

struct Blaze::Engine {
    TimePoint lastTime;

    Engine(const std::string& title, int width, int height, bool resizable) {
        display = std::make_unique<Display>(title, width, height, resizable);
        input = std::make_unique<Input>();
        device = std::make_unique<VulkanGfxDevice>(*display);
        renderer = std::make_unique<VulkanRenderer>(GetGfxDevice());
        ui = std::make_unique<UserInterface>(renderer->swapchain.getFrameCount());
    }

    ~Engine() {
        ui.reset();
        renderer.reset();
        input.reset();
        device.reset();
        display.reset();
    }

    auto ShouldQuit() -> bool {
        return display->shouldClose();
    }

    void WaitForLastPresentationAndGetTimestamp() {
        const auto currentTime = Clock::now();
        const auto delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        Time::_setDeltaTime(delta);
    }

    void PumpOSMessages() {
        display->pollEvents();
    }

    void UpdateInput() {

    }

    void Update(Blaze::App& app) {
        ui->setMousePosition(display->getMousePosition());
        ui->setDisplayScale(display->getScale());
        ui->setDisplaySize(display->getSize());
        ui->setDeltaTime(Time::getDeltaTime());

        for (int i = 0; i < 5; i++) {
            ui->SetMousePressed(i, display->getMousePressed(i));
        }

        input->_update();

        app.Update();
    }

    void WaitForRenderThread() {

    }

    void IssueRenderingCommands(Blaze::App& app) {
        ui->begin();
        app.DrawUI();
        ui->end();

        auto cmd = renderer->swapchain.begin(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
        app.Render(cmd);
        ui->draw(cmd);
        renderer->swapchain.present();
    }

    void Run(Blaze::App& app) {
        app.Init();

        lastTime = Clock::now();
        while (!ShouldQuit()) {
            WaitForLastPresentationAndGetTimestamp();
            PumpOSMessages();
            UpdateInput();
            Update(app);
            WaitForRenderThread();
            IssueRenderingCommands(app);
        }
        GetGfxDevice().WaitIdle();
        app.Destroy();
    }
};

void Blaze::Start(const std::string& title, int width, int height, bool resizable, App& app) {
    auto engine = Engine{title, width, height, resizable};
    engine.Run(app);
}

extern void Start(int argc, char* argv[]);

auto main(int argc, char* argv[]) -> int {
    Start(argc, argv);
    return 0;
}