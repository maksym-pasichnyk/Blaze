#include "Screen.hpp"
#include "Display.hpp"

extern auto GetDisplay() -> Display&;

auto Screen::getSize() -> glm::ivec2 {
    return GetDisplay().getSize();
}
