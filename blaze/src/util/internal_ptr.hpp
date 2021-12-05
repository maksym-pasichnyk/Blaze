#pragma once

#include <memory>
#include <functional>

namespace blaze {
    template<typename T>
    struct shared_internal_ptr;

    template<typename T>
    struct unique_internal_ptr;

    template<typename T>
    struct shared_internal_ptr : private std::shared_ptr<T> {
        using Base = std::shared_ptr<T>;
        using Base::Base;
        using Base::operator->;

        explicit shared_internal_ptr(unique_internal_ptr<T>&& unique) noexcept : Base(std::move(unique)) {}
    };
    template<typename T>
    shared_internal_ptr(unique_internal_ptr<T>) -> shared_internal_ptr<T>;

    template<typename T>
    struct unique_internal_ptr : private std::unique_ptr<T, std::function<void(T*)>> {
        friend struct shared_internal_ptr<T>;

        using Base = std::unique_ptr<T, std::function<void(T*)>>;
        using Base::Base;
        using Base::operator->;

        auto share() && -> shared_internal_ptr<T> {
            return shared_internal_ptr<T>(std::move(*this));
        }
    };

    template<typename T, typename... Args>
    static auto make_internal(Args &&... args) -> unique_internal_ptr<T> {
        return unique_internal_ptr<T>(new T(std::forward<Args>(args)...), [](T* p) {
            std::default_delete<T>{}(p);
        });
    }
}