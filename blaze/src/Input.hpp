#pragma once

#include <bitset>
#include <memory>
#include <glm/vec2.hpp>

enum class KeyCode {
    eTab,
    eLeftArrow,
    eRightArrow,
    eUpArrow,
    eDownArrow,
    ePageUp,
    ePageDown,
    eHome,
    eEnd,
    eInsert,
    eDelete,
    eBackspace,
    eSpace,
    eEnter,
    eEscape,
    eKeyPadEnter,
    eA,
    eC,
    eV,
    eX,
    eY,
    eZ,

    eLeftControl,
    eRightControl,
    eLeftShift,
    eRightShift,
    eLeftAlt,
    eRightAlt,
    eLeftSuper,
    eRightSuper
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
    std::bitset<256> thisFrameKeys{};
    std::bitset<256> lastFrameKeys{};
};