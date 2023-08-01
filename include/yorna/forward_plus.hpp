#ifndef YORNA_FORWARD_PLUS
#define YORNA_FORWARD_PLUS

#include <yorna/early_depth.hpp>
#include <yorna/light_culling.hpp>
#include <yorna/sunlight_shadow.hpp>

namespace yorna {

    class ForwardPlus : SharedInstance {
    public:
        ForwardPlus(const SharedInstance& instance, EarlyDepth& earlyDepth, SunlightShadow& sunlightShadow, LightCulling& lightCulling);
        ~ForwardPlus() noexcept = default;

        void resize(uint32_t width, uint32_t height);

        void begin(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;
        void rebind(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;
        void push(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& transform, const glm::mat4& modelNormal) const noexcept;
        void bind(const coffee::graphics::CommandBuffer& commandBuffer, const coffee::graphics::DescriptorSetPtr& materials) const noexcept;
        void end(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;

        void nextFrame() noexcept;
        uint32_t currentFrame() const noexcept;

        coffee::graphics::ImagePtr image;
        coffee::graphics::ImageViewPtr view;
        coffee::graphics::RenderPassPtr renderPass;

    private:
        struct RenderingPushConstants {
            glm::mat4 transform { 1.0f };
            glm::mat4 normal { 1.0f };
        };

        uint32_t frameIndex = 0;
        uint32_t outputWidth = 0;
        uint32_t outputHeight = 0;

        EarlyDepth& earlyDepth;
        SunlightShadow& sunlightShadow;
        LightCulling& lightCulling;

        coffee::graphics::FramebufferPtr framebuffer;
        PerFlightFrame<coffee::graphics::DescriptorSetPtr> descriptors;

        coffee::graphics::GraphicsPipelinePtr pipeline;
        coffee::graphics::DescriptorLayoutPtr renderingLayout;
        coffee::graphics::DescriptorLayoutPtr materialsLayout;
        coffee::graphics::SamplerPtr sunlightShadowSampler;
    };

} // namespace yorna

#endif