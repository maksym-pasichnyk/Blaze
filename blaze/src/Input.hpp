#pragma once

#include <bitset>
#include <memory>
#include <glm/vec2.hpp>

enum class KeyCode {
    /* Printable keys */
    eSpace,
    eApostrophe,
    eComma,
    eMinus,
    ePeriod,
    eSlash,
    e0,
    e1,
    e2,
    e3,
    e4,
    e5,
    e6,
    e7,
    e8,
    e9,
    eSemicolon,
    eEqual,
    eA,
    eB,
    eC,
    eD,
    eE,
    eF,
    eG,
    eH,
    eI,
    eJ,
    eK,
    eL,
    eM,
    eN,
    eO,
    eP,
    eQ,
    eR,
    eS,
    eT,
    eU,
    eV,
    eW,
    eX,
    eY,
    eZ,
    eLelfBracket,
    eBackslash,
    eRightBracket,
    eGraceAccent,
    eWorld1,
    eWorld2,
    /* Function keys */
    eEscape,
    eEnter,
    eTab,
    eBackspace,
    eInsert,
    eDelete,
    eRight,
    eLeft,
    eDown,
    eUp,
    ePageUp,
    ePageDown,
    eHome,
    eEnd,
    eCapsLock,
    eScrollLock,
    eNumLock,
    ePrintScreen,
    ePause,
    eF1,
    eF2,
    eF3,
    eF4,
    eF5,
    eF6,
    eF7,
    eF8,
    eF9,
    eF10,
    eF11,
    eF12,
    eF13,
    eF14,
    eF15,
    eF16,
    eF17,
    eF18,
    eF19,
    eF20,
    eF21,
    eF22,
    eF23,
    eF24,
    eF25,
    eKeyPad0,
    eKeyPad1,
    eKeyPad2,
    eKeyPad3,
    eKeyPad4,
    eKeyPad5,
    eKeyPad6,
    eKeyPad7,
    eKeyPad8,
    eKeyPad9,
    eKeyPadDecimal,
    eKeyPadDivide,
    eKeyPadMultiply,
    eKeyPadSubstract,
    eKeyPadAdd,
    eKeyPadEnter,
    eKeyPadEqual,
    eLeftShift,
    eLeftControl,
    eLeftAlt,
    eLeftSuper,
    eRightShift,
    eRightControl,
    eRightAlt,
    eRightSuper,
    eMenu
};

enum class MouseButton {
    Left,
    Right,
    Middle
};

struct Input {
    friend struct Blaze;

    static void update();
    static void setLock(bool flag);
    static auto getMouseDelta() -> glm::vec2;
    static auto getMousePosition() -> glm::ivec2;
    static auto isKeyUp(KeyCode keycode) -> bool;
    static auto isKeyDown(KeyCode keycode) -> bool;
    static auto isKeyPressed(KeyCode keycode) -> bool;
    static auto isMouseButtonUp(MouseButton button) -> bool;
    static auto isMouseButtonDown(MouseButton button) -> bool;
    static auto isMouseButtonPressed(MouseButton button) -> bool;

private:
    void _update();
    void _setLock(bool flag);
    auto _getMouseDelta() -> glm::vec2;
    auto _getMousePosition() -> glm::ivec2;
    auto _isKeyUp(KeyCode keycode) -> bool;
    auto _isKeyDown(KeyCode keycode) -> bool;
    auto _isKeyPressed(KeyCode keycode) -> bool;
    auto _isMouseButtonUp(MouseButton button) -> bool;
    auto _isMouseButtonDown(MouseButton button) -> bool;
    auto _isMouseButtonPressed(MouseButton button) -> bool;

private:
    bool lock;
    glm::vec2 mouseDelta;
    glm::vec2 mousePosition;
    std::bitset<5> thisFrameMouse{};
    std::bitset<5> lastFrameMouse{};
    std::bitset<128> thisFrameKeys{};
    std::bitset<128> lastFrameKeys{};
};