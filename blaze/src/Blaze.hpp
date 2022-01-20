#pragma once

#include <functional>
#include "CommandBuffer.hpp"

struct Blaze {
    struct Application {
        virtual ~Application() = default;

        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void Update() = 0;
        virtual void DrawUI() = 0;
        virtual void Draw(CommandBuffer cmd) = 0;
    };

    static void Start(std::function<std::unique_ptr<Application>()> const& fn);
};