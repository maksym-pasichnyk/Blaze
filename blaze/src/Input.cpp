#include "Input.hpp"
#include "Display.hpp"

extern auto GetDisplay() -> Display&;
extern auto GetInputDevice() -> Input&;

auto Input::getMouseDelta() -> glm::vec2 {
    return GetInputDevice().mouseDelta;
}

void Input::Update() {
    const auto center = glm::vec2(GetDisplay().getSize()) / 2.0f;
    mousePosition = GetDisplay().getMousePosition();
    mouseDelta = mousePosition - center;
    GetDisplay().setMousePosition(center);

    const auto keycodes = std::array {
        KeyCode::eLeftArrow,
        KeyCode::eRightArrow,
        KeyCode::eUpArrow,
        KeyCode::eDownArrow,
        KeyCode::eEscape,
        KeyCode::eSpace,
        KeyCode::eLeftShift
    };

    lastFrameKeys = thisFrameKeys;
    for (auto keycode : keycodes) {
        thisFrameKeys.set(size_t(keycode), GetDisplay().getKeyPressed(keycode));
    }

    lastFrameMouse = thisFrameMouse;
    for (auto button : {MouseButton::Left, MouseButton::Right, MouseButton::Middle}) {
        thisFrameMouse.set(size_t(button), GetDisplay().getMousePressed(button));
    }
}
auto Input::getMousePosition() -> glm::ivec2 {
    return GetInputDevice().mousePosition;
}
auto Input::isMouseButtonPressed(MouseButton button) -> bool {
    return GetInputDevice().thisFrameMouse.test(static_cast<int>(button));
}
auto Input::isMouseButtonDown(MouseButton button) -> bool {
    const auto lastFrame = GetInputDevice().lastFrameMouse.test(static_cast<int>(button));
    const auto thisFrame = GetInputDevice().thisFrameMouse.test(static_cast<int>(button));
    return thisFrame && !lastFrame;
}
auto Input::isMouseButtonUp(MouseButton button) -> bool {
    const auto lastFrame = GetInputDevice().lastFrameMouse.test(static_cast<int>(button));
    const auto thisFrame = GetInputDevice().thisFrameMouse.test(static_cast<int>(button));
    return !thisFrame && lastFrame;
}
auto Input::isKeyPressed(KeyCode keycode) -> bool {
    return GetInputDevice().thisFrameKeys.test(static_cast<int>(keycode));
}
auto Input::isKeyDown(KeyCode keycode) -> bool {
    const auto lastFrame = GetInputDevice().lastFrameKeys.test(static_cast<int>(keycode));
    const auto thisFrame = GetInputDevice().thisFrameKeys.test(static_cast<int>(keycode));
    return thisFrame && !lastFrame;
}
auto Input::isKeyUp(KeyCode keycode) -> bool {
    const auto lastFrame = GetInputDevice().lastFrameKeys.test(static_cast<int>(keycode));
    const auto thisFrame = GetInputDevice().thisFrameKeys.test(static_cast<int>(keycode));
    return !thisFrame && lastFrame;
}
