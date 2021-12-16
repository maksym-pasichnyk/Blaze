#include "Input.hpp"
#include "Display.hpp"

extern auto GetDisplay() -> Display&;
extern auto GetInputDevice() -> Input&;

auto Input::getMouseDelta() -> glm::vec2 {
    return GetInputDevice().mouseDelta;
}

void Input::_update() {
    const auto center = glm::vec2(GetDisplay().getSize()) / 2.0f;
    mouseDelta = GetDisplay().getMousePosition() - center;
    GetDisplay().setMousePosition(center);
}
