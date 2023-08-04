#ifndef YORNA_SKYBOX
#define YORNA_SKYBOX

#include <yorna/early_depth.hpp>
#include <yorna/forward_plus.hpp>

#include <coffee/graphics/mesh.hpp>

namespace yorna {

    class Skybox : SharedInstance {
    public:
        struct PushConstants {
            alignas(64) glm::mat4 projection {};
            alignas(64) glm::mat4 view {};
        };

        Skybox(const SharedInstance& instance, EarlyDepth& earlyDepth, ForwardPlus& forwardPlus);
        ~Skybox() noexcept = default;

        void perform(const coffee::graphics::CommandBuffer& commandBuffer);

    private:
        coffee::graphics::MeshPtr cubeMesh;
        coffee::graphics::ImagePtr cubeImage;
        coffee::graphics::ImageViewPtr cubeImageView;
        coffee::graphics::SamplerPtr sampler;

        EarlyDepth& earlyDepth;
        coffee::graphics::DescriptorLayoutPtr layout;
        PerFlightFrame<coffee::graphics::DescriptorSetPtr> descriptors;

        coffee::graphics::GraphicsPipelinePtr pipeline;
    };

} // namespace yorna

#endif
