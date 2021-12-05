#pragma once

#include <vulkan/vulkan.hpp>
#include <util/internal_ptr.hpp>

struct Timestamp;
struct UserInterface {
    friend struct Blaze;

    void setDisplaySize(std::pair<int, int> size);
    void setDisplayScale(std::pair<float, float> scale);
    void setDeltaTime(const Timestamp& delta);
    void setMousePosition(std::pair<float, float> pos);
    void setMousePressed(int button, bool flag);

    void begin();
    void end();
    void draw(vk::CommandBuffer cmd);

private:
    void init();

private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};
