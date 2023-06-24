#include <systems/shadows/sunlight_shadow.hpp>

#include <coffee/objects/vertex.hpp>

namespace editor {

    static constexpr std::array<VkClearValue, 1> depthClearValues = {
        VkClearValue { .depthStencil = { 0.0f, 0U } }
    };

    SunLightShadow::SunLightShadow(
        const coffee::graphics::DevicePtr& device,
        const coffee::AssetManagerPtr& assetManager,
        const coffee::FilesystemPtr& filesystem,
        uint32_t mapSize
    )
        : device_ { device }
        , mapSize_ { mapSize }
    {
        constexpr float ortho = 25.0f;
        sunlightCamera_.setOrthographicProjection(-ortho, ortho, -ortho, ortho, -ortho, ortho);
        sunlightCamera_.setViewDirection(glm::vec3 { 0.0f, 0.0f, 0.0f }, sunlight_.direction);

        shadowSampler_ = coffee::graphics::Sampler::create(device_, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .minLod = 0.0f,
            .maxLod = 1.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
        });

        depthMap_ = coffee::graphics::Image::create(device_, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device_->optimalDepthFormat(),
            .extent { mapSize, mapSize, 1U },
            .mipLevels = 1U,
            .arrayLayers = 1U,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .priority = 1.0f
        });

        depthMapView_ = coffee::graphics::ImageView::create(depthMap_, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = device_->optimalDepthFormat(),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        });

        renderPass_ = coffee::graphics::RenderPass::create(device_, {
            .depthStencilAttachment = coffee::graphics::AttachmentConfiguration {
                .format = device_->optimalDepthFormat(),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
            }
        });

        framebuffer_ = coffee::graphics::Framebuffer::create(device_, renderPass_, {
            .extent = { mapSize, mapSize },
            .layers = 1U,
            .depthStencilView = depthMapView_
        });

        pipeline_ = coffee::graphics::Pipeline::create(device_, renderPass_, {
            .shaders = coffee::utils::moveList<coffee::graphics::ShaderPtr, std::vector<coffee::graphics::ShaderPtr>>({
                assetManager->getShader(filesystem, "shaders/shadow_mapping.vert.spv", VK_SHADER_STAGE_VERTEX_BIT)
            }),
            .inputBindings = { coffee::graphics::InputBinding {
                .binding = 0,
                .stride = sizeof(coffee::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = { coffee::graphics::InputElement {
                    .location = 0U,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = offsetof(coffee::Vertex, position)
                }}
            }},
            .constantRanges = { coffee::graphics::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT,
                .size = sizeof(PushConstants),
                .offset = 0U
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_FRONT_BIT,
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .depthBiasEnable = true,
                .depthBiasConstantFactor = 1.25f,
                .depthBiasSlopeFactor = 1.75f
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL
            }
        });
    }

    void SunLightShadow::beginRender(const coffee::graphics::CommandBuffer& commandBuffer) {
        commandBuffer.beginRenderPass(renderPass_, framebuffer_, { .extent = { mapSize_, mapSize_ } }, depthClearValues.size(), depthClearValues.data());

        commandBuffer.setViewport({
            .width = static_cast<float>(mapSize_),
            .height = static_cast<float>(mapSize_),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        });
        commandBuffer.setScissor({ .extent = { mapSize_, mapSize_ } });

        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
    }

    void SunLightShadow::endRender(const coffee::graphics::CommandBuffer& commandBuffer) {
        commandBuffer.endRenderPass();
    }

    DirectionalLight& SunLightShadow::lightObject() noexcept {
        return sunlight_;
    }

    Camera& SunLightShadow::camera() noexcept {
        return sunlightCamera_;
    }

    coffee::graphics::SamplerPtr SunLightShadow::shadowSampler() const noexcept {
        return shadowSampler_;
    }

    coffee::graphics::ImagePtr SunLightShadow::depthMap() const noexcept {
        return depthMap_;
    }

    coffee::graphics::ImageViewPtr SunLightShadow::depthMapView() const noexcept {
        return depthMapView_;
    }

    coffee::graphics::PipelinePtr SunLightShadow::pipeline() const noexcept {
        return pipeline_;
    }

}