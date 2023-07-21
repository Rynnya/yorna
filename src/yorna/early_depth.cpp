#include <yorna/early_depth.hpp>

#include <coffee/graphics/command_buffer.hpp>
#include <coffee/objects/vertex.hpp>

namespace yorna {

    // clang-format off

    EarlyDepth::EarlyDepth(const SharedInstance& instance, uint32_t width, uint32_t height) : SharedInstance { instance }
    {
        layout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT } }
        });
        
        auto writer = coffee::graphics::DescriptorWriter(layout);

        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            descriptors[index] = coffee::graphics::DescriptorSet::create(device, writer.addBuffer(0, mvpUniformBuffers[index]));
        }

        renderPass = coffee::graphics::RenderPass::create(device, {
            .depthStencilAttachment = {
                .format = device->optimalDepthFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            },
        });

        pipeline = coffee::graphics::GraphicsPipeline::create(device, renderPass, {
            .vertexShader = assetManager->getShader(filesystem, "shaders/early_depth.vert.spv"),
            .vertexPushConstants = {
                .size = sizeof(glm::mat4)
            },
            .layouts = {
                layout
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
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL
            }
        });

        resize(width, height);
    }

    void EarlyDepth::resize(uint32_t width, uint32_t height)
    {
        image = coffee::graphics::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->optimalDepthFormat(),
            .extent = { width, height, },
            .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .priority = 1.0f
        });

        view = coffee::graphics::ImageView::create(image, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = image->imageFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        });

        framebuffer = coffee::graphics::Framebuffer::create(device, renderPass, {
            .extent = { width, height },
            .depthStencilView = view
        });

        screenWidth = width;
        screenHeight = height;
    }

    // clang-format on

    void EarlyDepth::begin(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept
    {
        const VkRect2D renderArea = { .extent = { 
            .width = screenWidth, 
            .height = screenHeight, 
        } };

        const VkViewport viewport = {
            .width = static_cast<float>(screenWidth),
            .height = static_cast<float>(screenHeight),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        constexpr VkClearValue clearValues = { .depthStencil = {
            .depth = 0.0f,
            .stencil = 0U,
        } };

        commandBuffer.beginRenderPass(renderPass, framebuffer, renderArea, 1U, &clearValues);
        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);

        commandBuffer.bindPipeline(pipeline);
        commandBuffer.bindDescriptorSets(pipeline, descriptors[frameIndex]);
    }

    void EarlyDepth::push(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& modelMatrix) const noexcept
    {
        commandBuffer.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), &modelMatrix);
    }

    void EarlyDepth::end(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept { commandBuffer.endRenderPass(); }

    void EarlyDepth::nextFrame() noexcept { frameIndex = (frameIndex + 1) % coffee::graphics::Device::kMaxOperationsInFlight; }

    uint32_t EarlyDepth::currentFrame() const noexcept { return frameIndex; }

} // namespace yorna