#include "Resources.hpp"

#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>

#if __ANDROID__
#include <android/asset_manager.h>
#endif

static auto resolve(const std::string& filename) -> std::filesystem::path {
    const auto pos = filename.find_first_of(':');
    if (pos == std::string::npos) {
        return std::filesystem::path("assets") / filename;
    }
    return std::filesystem::path(filename.substr(0, pos)) / "assets" / filename.substr(pos + 1);
}

auto Resources::get(const std::string& filename) -> tl::optional<Resource> {
#if __ANDROID__
    extern auto AndroidPlatform_getAssets() -> AAssetManager*;

    auto assets = AndroidPlatform_getAssets();
    auto asset = AAssetManager_open(assets, filename.c_str(), AASSET_MODE_BUFFER);
    if (asset == nullptr) {
        return std::nullopt;
    }

    auto resource = Resource(static_cast<size_t>(AAsset_getLength(asset)));
    AAsset_read(asset, resource.bytes_for_write(), resource.size());
    AAsset_close(asset);
    return resource;
#else
    const auto path = resolve(filename);

    if (!std::filesystem::exists(path)) {
        spdlog::warn("Resource '{}' not found", filename);
        return tl::nullopt;
    }

    const auto file_size = std::filesystem::file_size(path);
    auto resource = Resource(static_cast<size_t>(file_size));

    auto bytes = resource.bytes_for_overwrite();

    auto file = std::ifstream{path, std::ios::binary};
    file.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    return resource;
#endif
}