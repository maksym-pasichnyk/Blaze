#include "Time.hpp"
#include "Blaze.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include "UserInterface.hpp"

#include <VulkanGfxDevice.hpp>
#include <VulkanSwapchain.hpp>

#include <chrono>
#include <imgui.h>

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

void Blaze::Start(std::function<std::unique_ptr<Application>()> const& fn) {
    display = std::make_unique<Display>("Blaze", 800, 600, false);
    input = std::make_unique<Input>();
    device = std::make_unique<VulkanGfxDevice>(*display);
    swapchain = std::make_unique<VulkanSwapchain>(*device);
    device->SetRenderPass(swapchain->getRenderPass());
    ui = std::make_unique<UserInterface>(swapchain->getFrameCount());

    display->OnCharCallback.connect([](char c) {
        ui->AddInputCharacter(c);
    });
    display->OnScrollCallback.connect([](float x, float y) {
        ui->AddScrollMouse(x, y);
    });

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
        KeyCode::eLeft,
        KeyCode::eRight,
        KeyCode::eUp,
        KeyCode::eDown,
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

        ui->SetCurrentContext();

        display->pollEvents();

        Input::update();

        ui->SetDisplayScale(display->getScale());
        ui->SetDisplaySize(display->getSize());
        ui->SetDeltaTime(Time::getDeltaTime());
        ui->SetMousePosition(Input::getMousePosition());

        for (auto button : buttons) {
            ui->SetMousePressed(int(button), display->getMousePressed(button));
        }
        for (auto keycode : keycodes) {
            ui->SetKeyPressed(int(keycode), display->getKeyPressed(keycode));
        }

        app->Update();

        ImGui::NewFrame();
        app->DrawUI();
        ImGui::Render();

        auto cmd = swapchain->begin(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
        app->Draw(cmd);
        ui->Draw(cmd);
        swapchain->present();
    }
    device->WaitIdle();
    app->Destroy();
}