#pragma once

#include "Resource.hpp"

#include <string>
#include <tl/optional.hpp>

struct InputStream;
struct Resources {
    static auto get(const std::string& filename) -> tl::optional<Resource>;
};