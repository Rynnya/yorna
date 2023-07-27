#ifndef YORNA_SHARED
#define YORNA_SHARED

#include <coffee/interfaces/asset_manager.hpp>

#include <glm/glm.hpp>

namespace yorna {

    struct MVPUniformBuffer {
        alignas(64) glm::mat4 projection {};
        alignas(64) glm::mat4 view {};
        alignas(64) glm::mat4 inverseView {};
        alignas(64) glm::vec4 ambientLightColor { 1.0f, 1.0f, 1.0f, 0.07f };
    };

    class SharedInstance : coffee::NonMoveable {
    public:
        explicit SharedInstance(const coffee::graphics::DevicePtr& device);
        SharedInstance(const SharedInstance& instance);

        const coffee::graphics::DevicePtr& device;
        coffee::AssetManagerPtr assetManager;

        // TODO: Split filesystems into different objects, like shaders, models, audio, etc
        coffee::FilesystemPtr filesystem;

        std::shared_ptr<coffee::graphics::BufferPtr[coffee::graphics::Device::kMaxOperationsInFlight]> mvpUniformBuffers;
    };
    
    template <typename T>
    using PerFlightFrame = std::array<T, coffee::graphics::Device::kMaxOperationsInFlight>;

} // namespace yorna

#endif
