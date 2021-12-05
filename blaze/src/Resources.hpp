#pragma once

#include "Resource.hpp"

#include <optional>
#include <string>

struct InputStream;
struct Resources {
    static auto get(const std::string& filename) -> std::optional<Resource>;
};