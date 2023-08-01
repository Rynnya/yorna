#include <yorna/skybox.hpp>

#include <coffee/graphics/vertex.hpp>

namespace yorna {

    // clang-format off

    Skybox::Skybox(const SharedInstance& instance, EarlyDepth& earlyDepth, ForwardPlus& forwardPlus)
        : SharedInstance { instance }
        , earlyDepth { earlyDepth }
    {
        cubeModel = assetManager->loadModel({
            .filesystem = filesystem,
            .path = "cube.cfa"
        });

        cubeImage = assetManager->loadImage({
            .filesystem = filesystem,
            .path = "nebula_skybox.basis",
        });

        cubeImageView = coffee::graphics::ImageView::create(cubeImage, {
            .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
            .format = cubeImage->imageFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = cubeImage->mipLevels,
                .baseArrayLayer = 0,
                .layerCount = cubeImage->arrayLayers
            },
        });

        sampler = coffee::graphics::Sampler::create(device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias = 0.0f,
            .anisotropyEnable = true,
            .maxAnisotropy = 256.0f,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
        });

        layout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT } },
            { 1, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT } }
        });

        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            auto writer = coffee::graphics::DescriptorWriter { layout };

            writer.addBuffer(0, mvpUniformBuffers[index]);
            writer.addImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cubeImageView, sampler);

            descriptors[index] = coffee::graphics::DescriptorSet::create(device, writer);
        }

        pipeline = coffee::graphics::GraphicsPipeline::create(device, forwardPlus.renderPass, {
            .vertexShader = assetManager->loadShader({
                .filesystem = filesystem,
                .path = "shaders/skybox.vert.spv",
                .entrypoint = "main"
            }),
            .fragmentShader = assetManager->loadShader({
                .filesystem = filesystem,
                .path = "shaders/skybox.frag.spv",
                .entrypoint = "main"
            }),
            .layouts = {
                layout
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
            },
            .depthStencilInfo = {
                .depthTestEnable = true,
                .depthWriteEnable = true,
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL
            }
        });
    }

    // clang-format on

    void Skybox::perform(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        commandBuffer.bindPipeline(pipeline);
        commandBuffer.bindDescriptorSets(pipeline, descriptors[earlyDepth.currentFrame()]);
        commandBuffer.bindModel(cubeModel);
        commandBuffer.drawModel(cubeModel);
    }

} // namespace yorna
