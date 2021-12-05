#pragma once

#include "BlazeInstance.hpp"
#include "DescriptorPool.hpp"
#include "CommandPool.hpp"
#include "Texture.hpp"

#include <util/internal_ptr.hpp>

struct RenderPipeline {
    friend struct Blaze;

    auto begin() -> CommandBuffer;
    void present();
    auto getRenderPass() const -> vk::RenderPass;
    auto getFrameCount() const -> size_t;
    auto getSurfaceExtent() const -> vk::Extent2D;
private:
    void init();

private:
    struct Impl;
    blaze::unique_internal_ptr<Impl> impl;
};