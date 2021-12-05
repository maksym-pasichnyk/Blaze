#include "DescriptorPool.hpp"

#include "Blaze.hpp"
#include "BlazeInstance.hpp"

struct DescriptorPool::Impl {
    vk::DescriptorPool pool;

    Impl(uint32_t maxSets, std::span<const vk::DescriptorPoolSize> poolSizes) {
        const auto createInfo = vk::DescriptorPoolCreateInfo{
            .maxSets = maxSets,
            .poolSizeCount = static_cast<uint32_t>(std::size(poolSizes)),
            .pPoolSizes = std::data(poolSizes)
        };

        pool = Blaze::GetLogicalDevice().createDescriptorPool(createInfo, nullptr);
    }

    ~Impl() {
        Blaze::GetLogicalDevice().destroyDescriptorPool(pool, nullptr);
    }

    auto allocate(vk::DescriptorSetLayout setLayout) -> vk::DescriptorSet {
        const auto allocateInfo = vk::DescriptorSetAllocateInfo {
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &setLayout
        };

        vk::DescriptorSet descriptorSet;
        Blaze::GetLogicalDevice().allocateDescriptorSets(&allocateInfo, &descriptorSet);
        return descriptorSet;
    }

    void free(vk::DescriptorSet descriptorSet) {
        Blaze::GetLogicalDevice().freeDescriptorSets(pool, 1, &descriptorSet);
    }
};

DescriptorPool::DescriptorPool(uint32_t maxSets, std::span<const vk::DescriptorPoolSize> poolSizes) {
    impl = blaze::make_internal<Impl>(maxSets, poolSizes);
}

auto DescriptorPool::allocate(vk::DescriptorSetLayout setLayout) -> vk::DescriptorSet {
    return impl->allocate(setLayout);
}

void DescriptorPool::free(vk::DescriptorSet descriptorSet) {
    impl->free(descriptorSet);
}