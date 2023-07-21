#ifndef YORNA_EARLY_DEPTH
#define YORNA_EARLY_DEPTH

#include <coffee/graphics/descriptors.hpp>
#include <coffee/graphics/framebuffer.hpp>
#include <coffee/graphics/graphics_pipeline.hpp>
#include <coffee/graphics/render_pass.hpp>

#include <yorna/shared_instance.hpp>

namespace yorna {

    class EarlyDepth : SharedInstance {
    public:
        EarlyDepth(const SharedInstance& instance, uint32_t width, uint32_t height);

        void resize(uint32_t width, uint32_t height);

        void begin(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;
        void push(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& modelMatrix) const noexcept;
        void end(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;

        void nextFrame() noexcept;
        uint32_t currentFrame() const noexcept;

        coffee::graphics::ImagePtr image;
        coffee::graphics::ImageViewPtr view;

    private:
        uint32_t frameIndex = 0;
        uint32_t screenWidth = 0;
        uint32_t screenHeight = 0;

        coffee::graphics::DescriptorLayoutPtr layout;
        PerFlightFrame<coffee::graphics::DescriptorSetPtr> descriptors;

        coffee::graphics::RenderPassPtr renderPass;
        coffee::graphics::GraphicsPipelinePtr pipeline;
        coffee::graphics::FramebufferPtr framebuffer;
    };

} // namespace yorna

#endif
