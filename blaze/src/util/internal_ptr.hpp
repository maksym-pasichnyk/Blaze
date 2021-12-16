#pragma once

#include <memory>
#include <functional>

namespace blaze {
    template<typename T>
    struct shared_internal_ptr;

    template<typename T, typename D = std::function<void(T*)>>
    struct unique_internal_ptr;

    template<typename T>
    struct shared_internal_ptr : private std::shared_ptr<T> {
        using Base = std::shared_ptr<T>;
        using Base::Base;
        using Base::operator->;
        using Base::get;

        template<typename U, typename D = std::function<void(T*)>>
        shared_internal_ptr(unique_internal_ptr<U, D>&& unique) : Base(std::move(unique)) {}
    };
    template<typename T>
    shared_internal_ptr(unique_internal_ptr<T>) -> shared_internal_ptr<T>;

    template<typename T, typename D>
    struct unique_internal_ptr : private std::unique_ptr<T, D> {
        friend struct shared_internal_ptr<T>;

        using Base = std::unique_ptr<T, D>;
        using Base::Base;
        using Base::operator->;
        using Base::get;
    };

    template<typename T, typename... Args>
    static auto make_internal(Args &&... args) -> unique_internal_ptr<T> {
        return unique_internal_ptr<T>(new T(std::forward<Args>(args)...), [](T* self) {
            std::default_delete<T>{}(self);
        });
    }
}