#include "Resources.hpp"

#include <spdlog/spdlog.h>
#include <filesystem>
#include <physfs.h>

#if __ANDROID__
#include <android/asset_manager.h>
#endif

static auto resolve(const std::string& filename) -> std::filesystem::path {
    const auto pos = filename.find(':');
    if (pos == std::string::npos) {
        return filename;//std::filesystem::path("assets") / filename;
    }
    return "assets" / std::filesystem::path(filename.substr(0, pos)) / filename.substr(pos + 1);
}

auto Resources::get(const std::string& filename) -> tl::optional<Resource> {
//#if __ANDROID__
//    extern auto AndroidPlatform_getAssets() -> AAssetManager*;
//
//    auto assets = AndroidPlatform_getAssets();
//    auto asset = AAssetManager_open(assets, filename.c_str(), AASSET_MODE_BUFFER);
//    if (asset == nullptr) {
//        return std::nullopt;
//    }
//
//    auto resource = Resource(static_cast<size_t>(AAsset_getLength(asset)));
//    AAsset_read(asset, resource.bytes_for_write(), resource.size());
//    AAsset_close(asset);
//    return resource;
//#else
    const auto path = resolve(filename);

    auto file = PHYSFS_openRead(path.c_str());
    if (!file) {
        spdlog::warn("Resource '{}' not found", filename);
        return tl::nullopt;
    }

    auto resource = Resource(static_cast<size_t>(PHYSFS_fileLength(file)));
    auto bytes = resource.bytes_for_overwrite();
    PHYSFS_readBytes(file, bytes.data(), bytes.size());
    PHYSFS_close(file);
    return resource;
//#endif
}

auto Resources::open(const std::string &path) -> tl::optional<std::shared_ptr<ResourceStream>> {
    if (auto file = std::make_shared<ResourceStream>(path); *file) {
        return file;
    }
    return tl::nullopt;
}
