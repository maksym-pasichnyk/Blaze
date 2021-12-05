#pragma once

#include <chrono>

struct Timestamp : std::chrono::high_resolution_clock::duration {
    auto seconds() const -> double {
        return std::chrono::duration<double>(*this).count();
    }
};