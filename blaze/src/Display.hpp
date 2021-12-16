#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <glm/vec2.hpp>
#include <vulkan/vulkan.hpp>
#include <util/internal_ptr.hpp>

struct GLFWwindow;
struct Display {
    Display(const std::string& title, int width, int height, bool resizable);
    ~Display();

public:
    void pollEvents();
    auto shouldClose() const -> bool;
    auto createSurface(vk::Instance instance) -> vk::SurfaceKHR;
    auto getInstanceExtensions() const-> std::vector<const char *>;
    auto getSize() const-> glm::ivec2;
    auto getScale() const-> glm::vec2;
    auto hasFocus() const-> bool;
    auto getMousePosition() const -> glm::vec2;
    void setMousePosition(const glm::vec2& pos);
    auto getMousePressed(int button) const -> bool;

private:
    GLFWwindow* _window;
};