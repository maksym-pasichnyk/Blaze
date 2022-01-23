#pragma once

#include "Delegate.hpp"

#include <list>

template<typename Signature>
struct Signal;

template<typename... Args>
struct Signal<void(Args...)> {
    using Delegate = ::Delegate<void(Args...)>;

    template<auto fn, typename Self>
    inline void connect(Self* self) {
        auto delegate = Delegate{};
        delegate.template bind<fn>(self);
        delegates.emplace_back(delegate);
    }

    inline void connect(void(*fn)(Args...)) {
        delegates.emplace_back(Delegate{fn});
    }

    template<typename T, typename Self>
    inline void connect(T* self, void(*fn)(Self*, Args...)) {
        delegates.emplace_back(Delegate{self, fn});
    }

    inline void clear() {
        delegates.clear();
    }

    template<typename... FwdArgs>
    inline void operator()(FwdArgs&&... args) const {
        for (auto fn = delegates.begin(); fn != delegates.end(); ++fn) {
            (*fn)(args...);
        }
    }

    [[nodiscard]] inline explicit operator bool() const {
        return !empty();
    }

    [[nodiscard]] inline auto empty() const -> bool {
        return delegates.empty();
    }

private:
    std::list<Delegate> delegates;
};