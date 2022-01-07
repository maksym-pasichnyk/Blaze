#pragma once

#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>

template <typename T>
struct Iter : T {
    template <typename Fn>
    inline constexpr auto any(Fn&& fn) && {
        return ranges::any_of(*this, std::forward<Fn>(fn));
    }

    template <typename Fn>
    inline constexpr auto all(Fn&& fn) && {
        return ranges::all_of(*this, std::forward<Fn>(fn));
    }


    template <typename Fn>
    inline constexpr auto where(Fn&& fn) && {
        return ::Iter{ranges::views::filter(*this, std::forward<Fn>(fn))};
    }

    template <typename Fn>
    inline constexpr auto map(Fn&& fn) && {
        return ::Iter{ranges::views::transform(*this, std::forward<Fn>(fn))};
    }

    template <typename U>
    inline constexpr auto zip(Iter<U> iter) && {
        return ::Iter{ranges::views::zip(*this, iter)};
    }

    template <typename Tp>
    inline constexpr auto to() && -> Tp {
        return ranges::to<Tp>(*this);
    }

    template <typename U>
    inline constexpr auto split(U&& val) && {
        return ::Iter{ranges::views::split(*this, std::forward<U>(val))};
    }

    inline constexpr auto collect() && {
        return ranges::to_vector(*this);
    }
};

template <typename T>
Iter(T) -> Iter<T>;

template<typename T>
inline auto cpp_iter(T& self) {
    return Iter{ranges::views::ref(self)};
}

template<typename T>
inline auto cpp_iter(T&& self) {
    return Iter{ranges::make_view_closure(std::forward<T>(self))};
}