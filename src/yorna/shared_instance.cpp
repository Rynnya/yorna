#include <yorna/shared_instance.hpp>

namespace yorna {

    SharedInstance::SharedInstance(const coffee::graphics::DevicePtr& device)
        : device { device }
        , assetManager { coffee::AssetManager::create(device) }
        , filesystem { coffee::Filesystem::create("nativefs/") }
        , mvpUniformBuffers { std::make_shared<coffee::graphics::BufferPtr[coffee::graphics::Device::kMaxOperationsInFlight]>() }
    {
        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            auto& mvpUniformBuffer = mvpUniformBuffers[index];

            // clang-format off
            mvpUniformBuffer = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(MVPUniformBuffer),
                .instanceCount = 1,
                .usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST
            });
            // clang-format on

            mvpUniformBuffer->memory<MVPUniformBuffer>() = {};
        }
    }

    SharedInstance::SharedInstance(const SharedInstance& instance)
        : device { instance.device }
        , assetManager { instance.assetManager }
        , filesystem { instance.filesystem }
        , mvpUniformBuffers { instance.mvpUniformBuffers }
    {}

} // namespace yorna