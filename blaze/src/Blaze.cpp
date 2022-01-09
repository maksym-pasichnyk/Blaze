#include "Time.hpp"
#include "Blaze.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include "UserInterface.hpp"
#include "GraphicsBuffer.hpp"

#include <VulkanGfxDevice.hpp>
#include <VulkanSwapchain.hpp>

namespace {
    std::unique_ptr<Input> input;
    std::unique_ptr<Display> display;
    std::unique_ptr<UserInterface> ui;
    std::unique_ptr<VulkanGfxDevice> device;
    std::unique_ptr<VulkanSwapchain> swapchain;
    float deltaTime = 0.0f;
}

auto Time::getDeltaTime() -> float {
    return deltaTime;
}

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

void Blaze::Start(std::function<auto() -> std::unique_ptr<Application>> const& fn) {
    display = std::make_unique<Display>("Blaze", 800, 600, false);
    input = std::make_unique<Input>();
    device = std::make_unique<VulkanGfxDevice>(*display);
    swapchain = std::make_unique<VulkanSwapchain>(*device);
    device->SetRenderPass(swapchain->getRenderPass());
    ui = std::make_unique<UserInterface>(swapchain->getFrameCount());

    std::atexit([] {
        ui.reset();
        swapchain.reset();
        device.reset();
        input.reset();
        display.reset();
    });

    constexpr auto buttons = std::array{
        MouseButton::Left,
        MouseButton::Right,
        MouseButton::Middle,
    };

    constexpr auto keycodes = std::array {
        KeyCode::eTab,
        KeyCode::eLeftArrow,
        KeyCode::eRightArrow,
        KeyCode::eUpArrow,
        KeyCode::eDownArrow,
        KeyCode::ePageUp,
        KeyCode::ePageDown,
        KeyCode::eHome,
        KeyCode::eEnd,
        KeyCode::eInsert,
        KeyCode::eDelete,
        KeyCode::eBackspace,
        KeyCode::eSpace,
        KeyCode::eEnter,
        KeyCode::eEscape,
        KeyCode::eKeyPadEnter,
        KeyCode::eA,
        KeyCode::eC,
        KeyCode::eV,
        KeyCode::eX,
        KeyCode::eY,
        KeyCode::eZ,
    };

    auto app = fn();
    app->Init();

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!display->shouldClose()) {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        display->pollEvents();

        Input::update();

        ui->setDisplayScale(display->getScale());
        ui->setDisplaySize(display->getSize());
        ui->setDeltaTime(Time::getDeltaTime());
        ui->setMousePosition(Input::getMousePosition());

        for (auto button : buttons) {
            ui->setMousePressed(int(button), display->getMousePressed(button));
        }
        for (auto keycode : keycodes) {
            ui->setKeyPressed(int(keycode), display->getKeyPressed(keycode));
        }

        app->Update();

        ui->begin();
        app->DrawUI();
        ui->end();

        auto cmd = swapchain->begin(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
        app->Draw(cmd);
        ui->draw(cmd);
        swapchain->present();
    }
    device->WaitIdle();
    app->Destroy();
}