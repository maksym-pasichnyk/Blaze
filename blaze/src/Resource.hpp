#pragma once

#include <span>
#include <memory>

struct Resource {
    friend struct Resources;

    Resource() = default;

    [[nodiscard]] auto size() const noexcept -> size_t {
        return _size;
    }

    [[nodiscard]] auto data() const noexcept -> const char* {
        return _data.get();
    }

    [[nodiscard]] auto bytes() const noexcept -> std::span<const char> {
        return {_data.get(), _size};
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return _size == 0;
    }

private:
    explicit Resource(size_t size) : _data(std::make_unique<char[]>(size)), _size(size) {}

    [[nodiscard]] auto bytes_for_overwrite() noexcept -> std::span<char> {
        return {_data.get(), _size};
    }

    std::unique_ptr<char[]> _data = nullptr;
    size_t _size = 0;
};
