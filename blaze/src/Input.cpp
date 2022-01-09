#include "Input.hpp"
#include "Display.hpp"

extern auto GetDisplay() -> Display&;
extern auto GetInputDevice() -> Input&;

auto Input::getMouseDelta() -> glm::vec2 {
    return GetInputDevice()._getMouseDelta();
}
auto Input::getMousePosition() -> glm::ivec2 {
    return GetInputDevice()._getMousePosition();
}
auto Input::isMouseButtonPressed(MouseButton button) -> bool {
    return GetInputDevice()._isMouseButtonPressed(button);
}
auto Input::isMouseButtonDown(MouseButton button) -> bool {
    return GetInputDevice()._isMouseButtonDown(button);
}
auto Input::isMouseButtonUp(MouseButton button) -> bool {
    return GetInputDevice()._isMouseButtonUp(button);
}
auto Input::isKeyPressed(KeyCode keycode) -> bool {
    return GetInputDevice()._isKeyPressed(keycode);
}
auto Input::isKeyDown(KeyCode keycode) -> bool {
    return GetInputDevice()._isKeyDown(keycode);
}
auto Input::isKeyUp(KeyCode keycode) -> bool {
    return GetInputDevice()._isKeyUp(keycode);
}
void Input::setLock(bool flag) {
    return GetInputDevice()._setLock(flag);
}

void Input::update() {
    GetInputDevice()._update();
}

void Input::_update() {
    mousePosition = GetDisplay().getMousePosition();
    if (lock) {
        lock = false;
        const auto center = glm::vec2(GetDisplay().getSize()) / 2.0f;
        mouseDelta = mousePosition - center;
        GetDisplay().setMousePosition(center);
    } else {
        mouseDelta = {};
    }
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

auto Input::_getMouseDelta() -> glm::vec2 {
    return mouseDelta;
}
auto Input::_getMousePosition() -> glm::ivec2 {
    return mousePosition;
}
auto Input::_isMouseButtonPressed(MouseButton button) -> bool {
    return thisFrameMouse.test(static_cast<int>(button));
}
auto Input::_isMouseButtonDown(MouseButton button) -> bool {
    const auto lastFrame = lastFrameMouse.test(static_cast<int>(button));
    const auto thisFrame = thisFrameMouse.test(static_cast<int>(button));
    return thisFrame && !lastFrame;
}
auto Input::_isMouseButtonUp(MouseButton button) -> bool {
    const auto lastFrame = lastFrameMouse.test(static_cast<int>(button));
    const auto thisFrame = thisFrameMouse.test(static_cast<int>(button));
    return !thisFrame && lastFrame;
}
auto Input::_isKeyPressed(KeyCode keycode) -> bool {
    return thisFrameKeys.test(static_cast<int>(keycode));
}
auto Input::_isKeyDown(KeyCode keycode) -> bool {
    const auto lastFrame = lastFrameKeys.test(static_cast<int>(keycode));
    const auto thisFrame = thisFrameKeys.test(static_cast<int>(keycode));
    return thisFrame && !lastFrame;
}
auto Input::_isKeyUp(KeyCode keycode) -> bool {
    const auto lastFrame = lastFrameKeys.test(static_cast<int>(keycode));
    const auto thisFrame = thisFrameKeys.test(static_cast<int>(keycode));
    return !thisFrame && lastFrame;
}
void Input::_setLock(bool flag) {
    lock = flag;
}

