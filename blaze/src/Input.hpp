#pragma once

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

struct Input {
    friend struct Blaze;

    static auto getMouseDelta() -> glm::vec2;

private:
    glm::vec2 mouseDelta;

    void _update();
};