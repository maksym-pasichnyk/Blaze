#include "Input.hpp"
#include "Display.hpp"

extern auto GetDisplay() -> Display&;
extern auto GetInputDevice() -> Input&;

static constexpr auto keycodes = std::array {
    KeyCode::eSpace,
    KeyCode::eApostrophe,
    KeyCode::eComma,
    KeyCode::eMinus,
    KeyCode::ePeriod,
    KeyCode::eSlash,
    KeyCode::e0,
    KeyCode::e1,
    KeyCode::e2,
    KeyCode::e3,
    KeyCode::e4,
    KeyCode::e5,
    KeyCode::e6,
    KeyCode::e7,
    KeyCode::e8,
    KeyCode::e9,
    KeyCode::eSemicolon,
    KeyCode::eEqual,
    KeyCode::eA,
    KeyCode::eB,
    KeyCode::eC,
    KeyCode::eD,
    KeyCode::eE,
    KeyCode::eF,
    KeyCode::eG,
    KeyCode::eH,
    KeyCode::eI,
    KeyCode::eJ,
    KeyCode::eK,
    KeyCode::eL,
    KeyCode::eM,
    KeyCode::eN,
    KeyCode::eO,
    KeyCode::eP,
    KeyCode::eQ,
    KeyCode::eR,
    KeyCode::eS,
    KeyCode::eT,
    KeyCode::eU,
    KeyCode::eV,
    KeyCode::eW,
    KeyCode::eX,
    KeyCode::eY,
    KeyCode::eZ,
    KeyCode::eLelfBracket,
    KeyCode::eBackslash,
    KeyCode::eRightBracket,
    KeyCode::eGraceAccent,
    KeyCode::eWorld1,
    KeyCode::eWorld2,
    KeyCode::eEscape,
    KeyCode::eEnter,
    KeyCode::eTab,
    KeyCode::eBackspace,
    KeyCode::eInsert,
    KeyCode::eDelete,
    KeyCode::eRight,
    KeyCode::eLeft,
    KeyCode::eDown,
    KeyCode::eUp,
    KeyCode::ePageUp,
    KeyCode::ePageDown,
    KeyCode::eHome,
    KeyCode::eEnd,
    KeyCode::eCapsLock,
    KeyCode::eScrollLock,
    KeyCode::eNumLock,
    KeyCode::ePrintScreen,
    KeyCode::ePause,
    KeyCode::eF1,
    KeyCode::eF2,
    KeyCode::eF3,
    KeyCode::eF4,
    KeyCode::eF5,
    KeyCode::eF6,
    KeyCode::eF7,
    KeyCode::eF8,
    KeyCode::eF9,
    KeyCode::eF10,
    KeyCode::eF11,
    KeyCode::eF12,
    KeyCode::eF13,
    KeyCode::eF14,
    KeyCode::eF15,
    KeyCode::eF16,
    KeyCode::eF17,
    KeyCode::eF18,
    KeyCode::eF19,
    KeyCode::eF20,
    KeyCode::eF21,
    KeyCode::eF22,
    KeyCode::eF23,
    KeyCode::eF24,
    KeyCode::eF25,
    KeyCode::eKeyPad0,
    KeyCode::eKeyPad1,
    KeyCode::eKeyPad2,
    KeyCode::eKeyPad3,
    KeyCode::eKeyPad4,
    KeyCode::eKeyPad5,
    KeyCode::eKeyPad6,
    KeyCode::eKeyPad7,
    KeyCode::eKeyPad8,
    KeyCode::eKeyPad9,
    KeyCode::eKeyPadDecimal,
    KeyCode::eKeyPadDivide,
    KeyCode::eKeyPadMultiply,
    KeyCode::eKeyPadSubstract,
    KeyCode::eKeyPadAdd,
    KeyCode::eKeyPadEnter,
    KeyCode::eKeyPadEqual,
    KeyCode::eLeftShift,
    KeyCode::eLeftControl,
    KeyCode::eLeftAlt,
    KeyCode::eLeftSuper,
    KeyCode::eRightShift,
    KeyCode::eRightControl,
    KeyCode::eRightAlt,
    KeyCode::eRightSuper,
    KeyCode::eMenu
};

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

