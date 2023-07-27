#include <yorna/forward_plus.hpp>

#include <coffee/graphics/command_buffer.hpp>
#include <coffee/objects/vertex.hpp>

namespace yorna {

    // clang-format off

    ForwardPlus::ForwardPlus(const SharedInstance& instance, EarlyDepth& earlyDepth, SunlightShadow& sunlightShadow, LightCulling& lightCulling)
        : SharedInstance { instance }
        , earlyDepth { earlyDepth }
        , sunlightShadow { sunlightShadow }
        , lightCulling { lightCulling }
    {
        renderPass = coffee::graphics::RenderPass::create(device, {
            .colorAttachments = { coffee::graphics::AttachmentConfiguration {
                .format = device->surfaceFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            }},
            .depthStencilAttachment = {
                .format = device->optimalDepthFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            },
        });

        renderingLayout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT   } },
            { 1, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,           VK_SHADER_STAGE_FRAGMENT_BIT                                } },
            { 2, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,   VK_SHADER_STAGE_FRAGMENT_BIT                                } },
            { 3, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,           VK_SHADER_STAGE_FRAGMENT_BIT                                } },
            { 4, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,           VK_SHADER_STAGE_FRAGMENT_BIT                                } },
            { 5, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           VK_SHADER_STAGE_FRAGMENT_BIT                                } },
            { 6, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           VK_SHADER_STAGE_FRAGMENT_BIT                                } },
            { 7, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,            VK_SHADER_STAGE_FRAGMENT_BIT                                } },
            { 8, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,            VK_SHADER_STAGE_FRAGMENT_BIT                                } }
        });

        materialsLayout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,           VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 1, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,   VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 2, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,   VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 3, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,   VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 4, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,   VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 5, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,   VK_SHADER_STAGE_FRAGMENT_BIT } },
        });

        pipeline = coffee::graphics::GraphicsPipeline::create(device, renderPass, {
            .vertexShader = assetManager->getShader(filesystem, "shaders/forward_plus.vert.spv"),
            .fragmentShader = assetManager->getShader(filesystem, "shaders/forward_plus.frag.spv"),
            .vertexPushConstants = {
                .size = sizeof(RenderingPushConstants)
            },
            .fragmentPushConstants = {
                .size = sizeof(RenderingPushConstants)
            },
            .fragmentSpecializationConstants = {
                coffee::graphics::SpecializationConstant { 0, lightCulling.kAmountOfPointLights },
                coffee::graphics::SpecializationConstant { 1, lightCulling.kAmountOfSpotLights  }
            },
            .layouts = {
                renderingLayout,
                materialsLayout
            },
            .inputBindings = { coffee::graphics::InputBinding {
                .binding = 0U,
                .stride = sizeof(coffee::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = coffee::Vertex::getElementDescriptions()
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_BACK_BIT
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_EQUAL
            }
        });

        sunlightShadowSampler = coffee::graphics::Sampler::create(device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .minLod = 0.0f,
            .maxLod = 1.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        });
    }

    void ForwardPlus::resize(uint32_t width, uint32_t height)
    {
        outputWidth = width;
        outputHeight = height;

        image = coffee::graphics::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->surfaceFormat(),
            .extent = { outputWidth, outputHeight, },
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
        });

        view = coffee::graphics::ImageView::create(image, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = device->surfaceFormat()
        });

        framebuffer = coffee::graphics::Framebuffer::create(device, renderPass, {
            .extent = { outputWidth, outputHeight },
            .colorViews = { view },
            .depthStencilView = earlyDepth.view
        });

        auto writer = coffee::graphics::DescriptorWriter(renderingLayout);

        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            auto& renderingDescriptor = descriptors[index];

            writer.addBuffer(0, mvpUniformBuffers[index]);
            writer.addBuffer(1, sunlightShadow.uniformBuffers[index]);
            writer.addImage (2, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, sunlightShadow.depthMapView, sunlightShadowSampler);
            writer.addBuffer(3, lightCulling.pointLightsBuffers[index]);
            writer.addBuffer(4, lightCulling.spotLightsBuffers[index]);
            writer.addBuffer(5, lightCulling.pointLightIndexLists[index]);
            writer.addBuffer(6, lightCulling.spotLightIndexLists[index]);
            writer.addImage (7, VK_IMAGE_LAYOUT_GENERAL, lightCulling.pointLightGridView);
            writer.addImage (8, VK_IMAGE_LAYOUT_GENERAL, lightCulling.spotLightGridView);

            if (renderingDescriptor == nullptr) {
                renderingDescriptor = coffee::graphics::DescriptorSet::create(device, writer);
            }
            else {
                renderingDescriptor->update(writer);
            }
        }
    }

    // clang-format on

    void ForwardPlus::begin(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept
    {
        COFFEE_ASSERT(outputWidth > 0 && outputHeight > 0, "Did you forgot to call 'resize' before using ForwardPlus?");

        const VkRect2D renderArea = { .extent = {
            .width = outputWidth,
            .height = outputHeight,
        } };

        const VkViewport viewport = {
            .width = static_cast<float>(outputWidth),
            .height = static_cast<float>(outputHeight),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        constexpr std::array<VkClearValue, 2> clearValues = { VkClearValue { .color = { { 0.0f, 0.0f, 0.0f, 1.0f } } },
                                                              VkClearValue { .depthStencil = { 0.0f, 0U } } };

        commandBuffer.beginRenderPass(renderPass, framebuffer, renderArea, clearValues.size(), clearValues.data());
        commandBuffer.bindPipeline(pipeline);
        commandBuffer.bindDescriptorSets(pipeline, descriptors[frameIndex]);
        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);
    }

    void ForwardPlus::push(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& transform, const glm::mat4& modelNormal)
        const noexcept
    {
        RenderingPushConstants pushConstants {};
        pushConstants.transform = transform;
        pushConstants.normal = modelNormal;

        commandBuffer.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(pushConstants), &pushConstants);
    }

    void ForwardPlus::bind(const coffee::graphics::CommandBuffer& commandBuffer, const coffee::graphics::DescriptorSetPtr& materials) const noexcept
    {
        commandBuffer.bindDescriptorSets(pipeline, materials, 1);
    }

    void ForwardPlus::end(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept { commandBuffer.endRenderPass(); }

    void ForwardPlus::nextFrame() noexcept { frameIndex = (frameIndex + 1) % coffee::graphics::Device::kMaxOperationsInFlight; }

    uint32_t ForwardPlus::currentFrame() const noexcept { return frameIndex; }

} // namespace yorna