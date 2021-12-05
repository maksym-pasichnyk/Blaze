#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <vulkan/vulkan.hpp>

#include "util/internal_ptr.hpp"

struct Display {
    friend struct Blaze;

private:
    void init(const std::string& title, int width, int height, bool resizable);

public:
    void pollEvents();
    auto shouldClose() -> bool;
    auto createSurface(vk::Instance instance) -> vk::SurfaceKHR;
    auto getInstanceExtensions() const-> std::vector<const char *>;
    auto getSize() const-> std::pair<int, int>;
    auto getScale() const-> std::pair<float, float>;
    auto hasFocus() const-> bool;
    auto getMousePosition() const -> std::pair<float, float>;
    void setMousePosition(std::pair<float, float> pos);
    auto getMousePressed(int button) const -> bool;

private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};