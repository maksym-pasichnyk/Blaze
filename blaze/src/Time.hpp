#pragma once

struct Time {
    friend struct Blaze;

    static auto getDeltaTime() -> float;

private:
    static void _setDeltaTime(float deltaTime);
};