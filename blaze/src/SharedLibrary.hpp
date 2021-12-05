#pragma once

#include <optional>
#include <string>
#include <memory>

struct SharedLibrary {
    static auto open(const std::string &name) noexcept -> std::optional<SharedLibrary>;

    template<typename T>
    auto get(const char* function) const noexcept -> T {
        return reinterpret_cast<T>(get(function));
    }

private:
    explicit SharedLibrary(void* handle) noexcept;

    auto get(const char* function) const noexcept -> void*;

    struct AutoClose {
        void operator()(void* handle) noexcept;
    };
    std::unique_ptr<void, AutoClose> m_library;
};