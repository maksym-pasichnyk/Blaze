#pragma once

struct Display;
struct CommandPool;
struct GraphicsFence;
struct CommandBuffer;

struct Graphics {
    static auto CreateGraphicsFence() -> GraphicsFence;
    static void WaitOnGraphicsFence(const GraphicsFence& fence);
    static void ExecuteCommandBuffer(const CommandBuffer& cmd, const GraphicsFence& fence);

    static auto CreateCommandPool() -> CommandPool;
};