#include "CommandPool.hpp"

#include "Blaze.hpp"

struct CommandPool::Impl {
    vk::CommandPool pool;

    Impl(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags flags) {
        const auto createInfo = vk::CommandPoolCreateInfo {
            .flags = flags,
            .queueFamilyIndex = queueFamilyIndex
        };
        pool = Blaze::GetLogicalDevice().createCommandPool(createInfo, nullptr);
    }
    ~Impl() {
        Blaze::GetLogicalDevice().destroyCommandPool(pool, nullptr);
    }

    auto allocate(vk::CommandBufferLevel level) -> CommandBuffer {
        const auto allocateInfo = vk::CommandBufferAllocateInfo{
            .commandPool = pool,
            .level = level,
            .commandBufferCount = 1
        };

        vk::CommandBuffer cmd;
        Blaze::GetLogicalDevice().allocateCommandBuffers(&allocateInfo, &cmd);
        return std::bit_cast<CommandBuffer>(cmd);
    }

    auto free(CommandBuffer cmd) {
        Blaze::GetLogicalDevice().freeCommandBuffers(pool, { *cmd });
    }
};

CommandPool::CommandPool(uint32_t queueFamilyIndex) {
    impl = blaze::make_internal<Impl>(queueFamilyIndex, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
}

auto CommandPool::allocate(vk::CommandBufferLevel level) -> CommandBuffer {
    return impl->allocate(level);
}

void CommandPool::free(CommandBuffer cmd) {
    impl->free(cmd);
}
