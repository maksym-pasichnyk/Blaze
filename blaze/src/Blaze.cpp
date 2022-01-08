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

    explicit VulkanRenderer(VulkanGfxDevice& device) : swapchain(device) {}
};

static std::unique_ptr<Display> display;
static std::unique_ptr<Input> input;
static std::unique_ptr<VulkanGfxDevice> device;
static std::unique_ptr<VulkanRenderer> renderer;
static std::unique_ptr<UserInterface> ui;
static std::unique_ptr<Blaze::Engine> engine;

auto GetDisplay() -> Display& {
    return *display;
}

auto GetGfxDevice() -> VulkanGfxDevice& {
    return *device;
}

auto GetInputDevice() -> Input& {
    return *input;
}

auto GetUserInterface() -> UserInterface& {
    return *ui;
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
        input->Update();
        Input::setLock(false);
    }

    void Update(Blaze::Application& app) {
        ui->setDisplayScale(display->getScale());
        ui->setDisplaySize(display->getSize());
        ui->setDeltaTime(Time::getDeltaTime());

        ui->setMousePosition(input->getMousePosition());
        ui->setMousePressed(0, display->getMousePressed(MouseButton::Left));
        ui->setMousePressed(1, display->getMousePressed(MouseButton::Right));
        ui->setMousePressed(2, display->getMousePressed(MouseButton::Middle));

        ui->setKeyPressed(int(KeyCode::eTab), display->getKeyPressed(KeyCode::eTab));
        ui->setKeyPressed(int(KeyCode::eLeftArrow), display->getKeyPressed(KeyCode::eLeftArrow));
        ui->setKeyPressed(int(KeyCode::eRightArrow), display->getKeyPressed(KeyCode::eRightArrow));
        ui->setKeyPressed(int(KeyCode::eUpArrow), display->getKeyPressed(KeyCode::eUpArrow));
        ui->setKeyPressed(int(KeyCode::eDownArrow), display->getKeyPressed(KeyCode::eDownArrow));
        ui->setKeyPressed(int(KeyCode::ePageUp), display->getKeyPressed(KeyCode::ePageUp));
        ui->setKeyPressed(int(KeyCode::ePageDown), display->getKeyPressed(KeyCode::ePageDown));
        ui->setKeyPressed(int(KeyCode::eHome), display->getKeyPressed(KeyCode::eHome));
        ui->setKeyPressed(int(KeyCode::eEnd), display->getKeyPressed(KeyCode::eEnd));
        ui->setKeyPressed(int(KeyCode::eInsert), display->getKeyPressed(KeyCode::eInsert));
        ui->setKeyPressed(int(KeyCode::eDelete), display->getKeyPressed(KeyCode::eDelete));
        ui->setKeyPressed(int(KeyCode::eBackspace), display->getKeyPressed(KeyCode::eBackspace));
        ui->setKeyPressed(int(KeyCode::eSpace), display->getKeyPressed(KeyCode::eSpace));
        ui->setKeyPressed(int(KeyCode::eEnter), display->getKeyPressed(KeyCode::eEnter));
        ui->setKeyPressed(int(KeyCode::eEscape), display->getKeyPressed(KeyCode::eEscape));
        ui->setKeyPressed(int(KeyCode::eKeyPadEnter), display->getKeyPressed(KeyCode::eKeyPadEnter));
        ui->setKeyPressed(int(KeyCode::eA), display->getKeyPressed(KeyCode::eA));
        ui->setKeyPressed(int(KeyCode::eC), display->getKeyPressed(KeyCode::eC));
        ui->setKeyPressed(int(KeyCode::eV), display->getKeyPressed(KeyCode::eV));
        ui->setKeyPressed(int(KeyCode::eX), display->getKeyPressed(KeyCode::eX));
        ui->setKeyPressed(int(KeyCode::eY), display->getKeyPressed(KeyCode::eY));
        ui->setKeyPressed(int(KeyCode::eZ), display->getKeyPressed(KeyCode::eZ));

        app.Update();
    }

    void WaitForRenderThread() {

    }

    void IssueRenderingCommands(Blaze::Application & app) {
        ui->begin();
        app.DrawUI();
        ui->end();

        auto cmd = renderer->swapchain.begin(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
        app.Draw(cmd);
        ui->draw(cmd);
        renderer->swapchain.present();
    }

    void Run(Blaze::Application& app) {
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

auto main(int argc, char* argv[]) -> int {
    display = std::make_unique<Display>("Blaze", 800, 600, false);
    input = std::make_unique<Input>();
    device = std::make_unique<VulkanGfxDevice>(*display);
    renderer = std::make_unique<VulkanRenderer>(GetGfxDevice());
    ui = std::make_unique<UserInterface>(renderer->swapchain.getFrameCount());
    engine = std::make_unique<Blaze::Engine>();

    std::atexit([] {
        ui.reset();
        renderer.reset();
        device.reset();
        input.reset();
        display.reset();
    });

    engine->Run(*Blaze::CreateApplication());
    return 0;
}