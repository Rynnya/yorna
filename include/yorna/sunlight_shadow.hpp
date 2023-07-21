#ifndef YORNA_SHADOW_MAPPING
#define YORNA_SHADOW_MAPPING

#include <coffee/coffee.hpp>

#include <entities/camera.hpp>
#include <entities/light_objects.hpp>

#include <yorna/shared_instance.hpp>

namespace yorna {

    class SunlightShadow : SharedInstance {
    public:
        struct PushConstants {
            glm::mat4 lightSpaceMatrix { 1.0f };
            glm::mat4 modelMatrix { 1.0f };
        };

        struct UniformBuffer {
            alignas(64) glm::mat4 sunlightSpaceMatrix { 1.0f };
            alignas(16) glm::vec4 sunlightDirection { 1.0f };
            alignas(16) glm::vec4 sunlightColor { 1.0f };
        };

        SunlightShadow(const SharedInstance& instance, uint32_t mapSize = 1024);
        ~SunlightShadow() noexcept = default;

        void begin(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;
        void push(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& modelMatrix) const noexcept;
        void end(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;

        void nextFrame() noexcept;
        uint32_t currentFrame() const noexcept;

        const uint32_t kMapSize;

        coffee::graphics::ImagePtr depthMap;
        coffee::graphics::ImageViewPtr depthMapView;

        Camera camera;
        DirectionalLight lightObject;
        PerFlightFrame<coffee::graphics::BufferPtr> uniformBuffers;

    private:
        mutable uint32_t frameIndex = 0;

        coffee::graphics::RenderPassPtr renderPass;
        coffee::graphics::GraphicsPipelinePtr pipeline;
        coffee::graphics::FramebufferPtr framebuffer;
    };

} // namespace yorna

#endif