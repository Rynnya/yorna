#include <yorna/sunlight_shadow.hpp>

#include <coffee/graphics/vertex.hpp>

namespace yorna {

    // clang-format off

    SunlightShadow::SunlightShadow(const SharedInstance& instance, uint32_t mapSize)
        : SharedInstance { instance }
        , kMapSize { mapSize }
        , lightObject { glm::vec3 { 1.0f }, glm::vec3 { 1.0f } }
        , camera {}
    {
        constexpr float ortho = 25.0f;
        camera.setOrthographicProjection(-ortho, ortho, -ortho, ortho, -ortho, ortho);
        camera.setViewDirection(glm::vec3 { 0.0f, 0.0f, 0.0f }, lightObject.direction);

        for (auto& sunlightUniformBuffer : uniformBuffers) {
            sunlightUniformBuffer = coffee::graphics::Buffer::create(device, {
                .instanceSize = static_cast<uint32_t>(sizeof(UniformBuffer)),
                .instanceCount = 1U,
                .usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            });

            SunlightShadow::UniformBuffer& ubo = sunlightUniformBuffer->memory<SunlightShadow::UniformBuffer>();

            ubo.sunlightSpaceMatrix = camera.projection() * camera.view();
            ubo.sunlightDirection = glm::vec4 { lightObject.direction, 1.0f };
            ubo.sunlightColor = glm::vec4 { lightObject.color, 1.0f };

            sunlightUniformBuffer->flush();
        }

        renderPass = coffee::graphics::RenderPass::create(device, {
            .depthStencilAttachment = {
                .format = device->optimalDepthFormat(),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            },
        });

        depthMap = coffee::graphics::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->optimalDepthFormat(),
            .extent = { mapSize, mapSize, 1U },
            .mipLevels = 1U,
            .arrayLayers = 1U,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .priority = 1.0f,
        });

        depthMapView = coffee::graphics::ImageView::create(depthMap, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = device->optimalDepthFormat(),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        });

        framebuffer = coffee::graphics::Framebuffer::create(device, renderPass, {
            .extent = { mapSize, mapSize },
            .layers = 1U,
            .depthStencilView = depthMapView,
        });

        pipeline = coffee::graphics::GraphicsPipeline::create(device, renderPass, {
            .vertexShader = assetManager->loadShader({
                .filesystem = filesystem,
                .path = "shaders/shadow_mapping.vert.spv",
                .entrypoint = "main"
            }),
            .vertexPushConstants = {
                .size = sizeof(PushConstants)
            },
            .inputBindings = { coffee::graphics::InputBinding {
                .binding = 0,
                .stride = sizeof(coffee::graphics::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = { coffee::graphics::InputElement {
                    .location = 0U,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = offsetof(coffee::graphics::Vertex, position),
                }},
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_FRONT_BIT,
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .depthBiasEnable = true,
                .depthBiasConstantFactor = 1.25f,
                .depthBiasSlopeFactor = 1.75f,
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
            },
        });
    }

    // clang-format on

    void SunlightShadow::begin(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept
    {
        const VkRect2D renderArea = { .extent = {
            .width = kMapSize,
            .height = kMapSize,
        } };

        const VkViewport viewport = {
            .width = static_cast<float>(kMapSize),
            .height = static_cast<float>(kMapSize),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        constexpr VkClearValue clearValues = { .depthStencil = { 
            .depth = 0.0f,
            .stencil = 0U,
        } };

        const glm::mat4 sunlightSpaceMatrix = camera.projection() * camera.view();

        commandBuffer.beginRenderPass(renderPass, framebuffer, renderArea, 1U, &clearValues);

        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);

        commandBuffer.bindPipeline(pipeline);
        commandBuffer.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &sunlightSpaceMatrix);
    }

    void SunlightShadow::push(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& modelMatrix) const noexcept
    {
        commandBuffer.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &modelMatrix, sizeof(glm::mat4));
    }

    void SunlightShadow::end(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept { commandBuffer.endRenderPass(); }

    void SunlightShadow::nextFrame() noexcept { frameIndex = (frameIndex + 1) % coffee::graphics::Device::kMaxOperationsInFlight; }

    uint32_t SunlightShadow::currentFrame() const noexcept { return frameIndex; }

} // namespace yorna