#ifndef EDITOR_SHADOW_MAPPING
#define EDITOR_SHADOW_MAPPING

#include <coffee/coffee.hpp>

#include <entities/camera.hpp>
#include <entities/light_objects.hpp>

namespace yorna {

    class SunLightShadow {
    public:
        struct PushConstants {
            glm::mat4 lightSpaceMatrix { 1.0f };
            glm::mat4 modelMatrix { 1.0f };
        };

        SunLightShadow(
            const coffee::graphics::DevicePtr& device,
            const coffee::AssetManagerPtr& assetManager,
            const coffee::FilesystemPtr& filesystem,
            uint32_t mapSize = 1024);
        ~SunLightShadow() noexcept = default;

        void beginRender(const coffee::graphics::CommandBuffer& commandBuffer);
        void endRender(const coffee::graphics::CommandBuffer& commandBuffer);

        DirectionalLight& lightObject() noexcept;
        Camera& camera() noexcept;

        coffee::graphics::SamplerPtr shadowSampler() const noexcept;
        coffee::graphics::ImagePtr depthMap() const noexcept;
        coffee::graphics::ImageViewPtr depthMapView() const noexcept;
        coffee::graphics::PipelinePtr pipeline() const noexcept;

    private:
        coffee::graphics::DevicePtr device_;
        uint32_t mapSize_;

        DirectionalLight sunlight_ { glm::vec3 { 1.0f, 1.0f, 1.0f } };
        Camera sunlightCamera_ {};

        coffee::graphics::SamplerPtr shadowSampler_;
        coffee::graphics::ImagePtr depthMap_;
        coffee::graphics::ImageViewPtr depthMapView_;

        coffee::graphics::RenderPassPtr renderPass_;
        coffee::graphics::FramebufferPtr framebuffer_;
        coffee::graphics::PipelinePtr pipeline_;
    };

}

#endif