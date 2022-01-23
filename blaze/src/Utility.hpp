#pragma once

#include <variant>

template <typename... Ts, typename... Fs>
inline constexpr auto match(std::variant<Ts...>& v, Fs&&... fs) -> decltype(auto) {
    struct matches : Fs... {
        using Fs::operator()...;
    };
    return std::visit(matches{ std::forward<Fs>(fs)... }, v);
}

template <typename... Ts, typename... Fs>
inline constexpr auto match(std::variant<Ts...>&& v, Fs&&... fs) -> decltype(auto) {
    struct matches : Fs... {
        using Fs::operator()...;
    };
    return std::visit(matches{ std::forward<Fs>(fs)... }, std::move(v));
}

template <typename... Ts, typename... Fs>
inline constexpr auto match(const std::variant<Ts...>& v, Fs&&... fs) -> decltype(auto) {
    struct matches : Fs... {
        using Fs::operator()...;
    };
    return std::visit(matches{ std::forward<Fs>(fs)... }, std::move(v));
}

template <typename To, typename From>
inline constexpr auto implicit_cast(From&& from) -> To {
    static_assert(std::is_convertible_v<From, To>);
    return std::forward<From>(from);
}

template<auto fn, typename Self, typename R, typename... Args>
inline constexpr auto as_static_function(auto(Self::*)(Args...) -> R) -> auto(*)(Self*, Args...) -> R {
    return [](Self* self, Args... args) -> R {
        return (self->*fn)(args...);
    };
}

template <auto fn>
inline constexpr auto as_static_function() {
    return as_static_function<fn>(fn);
}
