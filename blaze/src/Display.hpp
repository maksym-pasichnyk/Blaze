#pragma once

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <Input.hpp>
#include <glm/vec2.hpp>
#include <vulkan/vulkan.hpp>

struct GLFWwindow;
struct Display {
    Display(const std::string& title, int width, int height, bool resizable);
    ~Display();

public:
    void close();
    void pollEvents();
    auto shouldClose() const -> bool;
    auto createSurface(vk::Instance instance) -> vk::SurfaceKHR;
    auto getInstanceExtensions() const-> std::vector<const char *>;
    auto getSize() const-> glm::ivec2;
    auto getScale() const-> glm::vec2;
    auto hasFocus() const-> bool;
    auto getMousePosition() const -> glm::vec2;
    void setMousePosition(const glm::vec2& pos);
    auto getMousePressed(MouseButton button) const -> bool;
    auto getKeyPressed(KeyCode keycode) const -> bool;

private:
    std::array<int, 5> mouseButtonMapping;
    std::array<int, 128> keyCodeMapping;

    GLFWwindow* _window;
};