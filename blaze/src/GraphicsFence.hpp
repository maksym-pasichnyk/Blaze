#pragma once

#include <memory>

struct GraphicsFence {
    friend struct Graphics;
private:
    struct Dispose {
        void operator()(void*);
    };
    std::unique_ptr<void, Dispose> impl;
};