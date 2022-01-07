#pragma once

#include "Resource.hpp"
#include "physfs.h"

#include <string>
#include <istream>
#include <tl/optional.hpp>

struct ResourceStream : std::istream {
    explicit ResourceStream(const std::string& path) : std::istream(&buf) {
        if (!buf.open(path)) {
            setstate(ios_base::failbit);
        }
    }
    auto length() const -> size_t {
        return buf.length();
    }

private:
    struct filebuf : std::streambuf {
        filebuf() = default;
        ~filebuf() override {
            if (file != nullptr) {
                PHYSFS_close(file);
                file = nullptr;
            }
        }

        auto open(const std::string& path) -> bool {
            if (file != nullptr) { return false; }
            file = PHYSFS_openRead(path.c_str());
            return file != nullptr;
        }

        auto is_open() const -> bool {
            return file != nullptr;
        }

        auto underflow() -> int_type override {
            if (PHYSFS_eof(file)) {
                return traits_type::eof();
            }
            const auto len = PHYSFS_readBytes(file, tmp.data(), tmp.size());
            if (len < 1) {
                return traits_type::eof();
            }
            setg(tmp.data(), tmp.data(), tmp.data() + len);
            return traits_type::to_int_type(*gptr());
        }

        auto length() const -> size_t {
            return PHYSFS_fileLength(file);
        }

    private:
        PHYSFS_File* file = nullptr;
        std::array<char, 1024> tmp{};
    };

    filebuf buf{};
};

struct Resources {
    static auto get(const std::string& filename) -> tl::optional<Resource>;
    static auto open(const std::string& path) -> tl::optional<std::shared_ptr<ResourceStream>>;
};