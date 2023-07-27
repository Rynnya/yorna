#ifndef YORNA_LIGHT_CULLING
#define YORNA_LIGHT_CULLING

#include <coffee/graphics/compute_pipeline.hpp>

#include <entities/camera.hpp>
#include <entities/light_objects.hpp>

#include <yorna/early_depth.hpp>
#include <yorna/shared_instance.hpp>

namespace yorna {

    class LightCulling : SharedInstance {
    public:
        static constexpr uint32_t kFrustumBlockSize = 16U;
        static constexpr uint32_t kAverageOverlappingLightsPerTile = 512U;

        LightCulling(const SharedInstance& instance, EarlyDepth& earlyDepth, uint32_t pointLightCount, uint32_t spotLightCount);

        void resize(const Camera& camera, uint32_t width, uint32_t height);

        void reset(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept;
        void perform(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& viewMatrix) const noexcept;

        PerFlightFrame<coffee::graphics::BufferPtr> pointLightsBuffers;
        PerFlightFrame<coffee::graphics::BufferPtr> spotLightsBuffers;
        PerFlightFrame<coffee::graphics::BufferPtr> pointLightIndexLists;
        PerFlightFrame<coffee::graphics::BufferPtr> spotLightIndexLists;

        coffee::graphics::ImagePtr pointLightGrid;
        coffee::graphics::ImagePtr spotLightGrid;
        coffee::graphics::ImageViewPtr pointLightGridView;
        coffee::graphics::ImageViewPtr spotLightGridView;

        const uint32_t kAmountOfPointLights;
        const uint32_t kAmountOfSpotLights;

        void nextFrame() noexcept;
        uint32_t currentFrame() const noexcept;

    private:
        struct FrustumCalculationPushConstants {
            glm::mat4 inverseProjection {};
            glm::vec2 screenDimensions {};
            glm::uvec2 numThreads {};
        };

        struct LightCullingPushConstants {
            glm::mat4 inverseProjection {};
            glm::mat4 viewMatrix {};
        };

        struct OutputIndexCounters {
            uint32_t pointLightIndexCounter {};
            uint32_t spotLightIndexCounter {};
        };

        mutable uint32_t frameIndex = 0;
        uint32_t widthInPixels = 0;
        uint32_t heightInPixels = 0;
        uint32_t widthInTiles = 0;
        uint32_t heightInTiles = 0;
        glm::mat4 inverseProjection {};

        EarlyDepth& earlyDepth;

        coffee::graphics::BufferPtr calculatedFrustums;
        coffee::graphics::DescriptorLayoutPtr frustumsLayout;
        coffee::graphics::DescriptorSetPtr frustumsDescriptor;
        coffee::graphics::ComputePipelinePtr frustumsPipeline;
        coffee::graphics::FencePtr frustumsCompletionFence;

        coffee::graphics::BufferPtr defaultIndexCounters;
        PerFlightFrame<coffee::graphics::BufferPtr> indexCounters;

        coffee::graphics::DescriptorLayoutPtr lightCullingLayout;
        PerFlightFrame<coffee::graphics::DescriptorSetPtr> lightCullingDescriptors;

        coffee::graphics::ComputePipelinePtr lightCullingPipeline;
        coffee::graphics::SamplerPtr depthSampler;
        coffee::graphics::FencePtr layoutTranslationFence;
    };

} // namespace yorna

#endif