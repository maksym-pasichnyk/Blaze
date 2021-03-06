#pragma once

#include <memory>

struct CommandBuffer;
struct CommandPool {
    friend struct Graphics;

    auto allocate() -> CommandBuffer;
    void free(const CommandBuffer& cmd);

private:
    struct Dispose {
        void operator()(void*);
    };
    std::unique_ptr<void, Dispose> impl;
};