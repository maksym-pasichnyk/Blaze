#pragma once

#include <memory>

enum class BlazeKeyCode {
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
};