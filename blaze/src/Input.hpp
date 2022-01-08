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

    static auto getMouseDelta() -> glm::vec2;
    static auto getMousePosition() -> glm::ivec2;

    static auto isMouseButtonPressed(MouseButton button) -> bool;
    static auto isMouseButtonDown(MouseButton button) -> bool;
    static auto isMouseButtonUp(MouseButton button) -> bool;
    static auto isKeyPressed(KeyCode keycode) -> bool;
    static auto isKeyDown(KeyCode keycode) -> bool;
    static auto isKeyUp(KeyCode keycode) -> bool;
    static void setLock(bool flag);

private:
    void Update();

private:
    bool lock;
    glm::vec2 mouseDelta;
    glm::vec2 mousePosition;
    std::bitset<5> thisFrameMouse{};
    std::bitset<5> lastFrameMouse{};
    std::bitset<256> thisFrameKeys{};
    std::bitset<256> lastFrameKeys{};
};