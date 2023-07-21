#include <yorna/light_culling.hpp>

#include <coffee/graphics/command_buffer.hpp>

namespace yorna {

    // clang-format off

    LightCulling::LightCulling(const SharedInstance& instance, EarlyDepth& earlyDepth, uint32_t pointLightCount, uint32_t spotLightCount)
        : SharedInstance { instance }
        , kAmountOfPointLights { pointLightCount }
        , kAmountOfSpotLights { spotLightCount }
        , earlyDepth { earlyDepth }
    {
        frustumsLayout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT } }
        });

        frustumsPipeline = coffee::graphics::ComputePipeline::create(device, {
            .shader = assetManager->getShader(filesystem, "shaders/calculate_frustums.comp.spv"),
            .pushConstants = {
                .size = sizeof(FrustumCalculationPushConstants)
            },
            .layouts = {
                frustumsLayout
            }
        });

        frustumsCompletionFence = coffee::graphics::Fence::create(device);

        defaultIndexCounters = coffee::graphics::Buffer::create(device, {
            .instanceSize = sizeof(OutputIndexCounters),
            .instanceCount = 1,
            .usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        });

        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            pointLightsBuffers[index] = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(PointLight),
                .instanceCount = kAmountOfPointLights,
                .usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
            });

            spotLightsBuffers[index] = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(SpotLight),
                .instanceCount = kAmountOfSpotLights,
                .usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
            });

            indexCounters[index] = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(OutputIndexCounters),
                .instanceCount = 1,
                .usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
            });
        }

        lightCullingLayout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,   VK_SHADER_STAGE_COMPUTE_BIT } },
            { 1, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           VK_SHADER_STAGE_COMPUTE_BIT } },
            { 2, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,           VK_SHADER_STAGE_COMPUTE_BIT } },
            { 3, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,           VK_SHADER_STAGE_COMPUTE_BIT } },
            { 4, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           VK_SHADER_STAGE_COMPUTE_BIT } },
            { 5, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           VK_SHADER_STAGE_COMPUTE_BIT } },
            { 6, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           VK_SHADER_STAGE_COMPUTE_BIT } },
            { 7, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,            VK_SHADER_STAGE_COMPUTE_BIT } },
            { 8, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,            VK_SHADER_STAGE_COMPUTE_BIT } },
        });

        lightCullingPipeline = coffee::graphics::ComputePipeline::create(device, {
            .shader = assetManager->getShader(filesystem, "shaders/light_culling.comp.spv"),
            .pushConstants = {
                .size = sizeof(LightCullingPushConstants)
            },
            .specializationConstants = {
                coffee::graphics::SpecializationConstant { 0, kAmountOfPointLights },
                coffee::graphics::SpecializationConstant { 1, kAmountOfSpotLights  }
            },
            .layouts = {
                lightCullingLayout
            }
        });

        depthSampler = coffee::graphics::Sampler::create(device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
        });

        layoutTranslationFence = coffee::graphics::Fence::create(device);
    }

    void LightCulling::resize(const Camera& camera, uint32_t width, uint32_t height)
    {
        device->waitDeviceIdle();

        widthInPixels = width;
        heightInPixels = height;
        widthInTiles = static_cast<uint32_t>(glm::ceil(static_cast<float>(width) / kFrustumBlockSize));
        heightInTiles = static_cast<uint32_t>(glm::ceil(static_cast<float>(height) / kFrustumBlockSize));
        inverseProjection = camera.computeInverseProjection();

        calculatedFrustums = coffee::graphics::Buffer::create(device, {
            .instanceSize = 4 * sizeof(glm::vec4), // 4 planes, each plane is vec4
            .instanceCount = widthInTiles * heightInTiles,
            .usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        });

        auto frustumWriter = coffee::graphics::DescriptorWriter(frustumsLayout);
        frustumWriter.addBuffer(0, calculatedFrustums);

        if (frustumsDescriptor == nullptr) {
            frustumsDescriptor = coffee::graphics::DescriptorSet::create(device, frustumWriter);
        }
        else {
            frustumsDescriptor->update(frustumWriter);
        }
        
        FrustumCalculationPushConstants pushConstants {};
        pushConstants.inverseProjection = inverseProjection;
        pushConstants.screenDimensions = glm::vec2 { widthInPixels, heightInPixels };
        pushConstants.numThreads = glm::uvec2 { widthInTiles, heightInTiles };

        auto frustumsCommandBuffer = coffee::graphics::CommandBuffer::createCompute(device);

        frustumsCommandBuffer.bindPipeline(frustumsPipeline);
        frustumsCommandBuffer.bindDescriptorSets(frustumsPipeline, frustumsDescriptor);
        frustumsCommandBuffer.pushConstants(frustumsPipeline, sizeof(FrustumCalculationPushConstants), &pushConstants);

        constexpr float kBlockSizeAsFloat = static_cast<float>(kFrustumBlockSize);
        frustumsCommandBuffer.dispatch(glm::ceil(widthInTiles / kBlockSizeAsFloat), glm::ceil(heightInTiles / kBlockSizeAsFloat), 1);

        device->submit(std::move(frustumsCommandBuffer), {}, frustumsCompletionFence, true);

        std::array<VkImageMemoryBarrier, 2U * coffee::graphics::Device::kMaxOperationsInFlight> layoutBarriers {};

        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            auto& pointLightIndexList = pointLightIndexLists[index];
            auto& spotLightIndexList = spotLightIndexLists[index];
            auto& pointLightGrid = pointLightGrids[index];
            auto& spotLightGrid = spotLightGrids[index];
            auto& pointLightGridView = pointLightGridViews[index];
            auto& spotLightGridView = spotLightGridViews[index];

            pointLightIndexList = nullptr;
            spotLightIndexList = nullptr;
            pointLightGrid = nullptr;
            spotLightGrid = nullptr;
            pointLightGridView = nullptr;
            spotLightGridView = nullptr;

            pointLightIndexList = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(uint32_t),
                .instanceCount = widthInTiles * heightInTiles * kAverageOverlappingLightsPerTile,
                .usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
            });

            spotLightIndexList = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(uint32_t),
                .instanceCount = widthInTiles * heightInTiles * kAverageOverlappingLightsPerTile,
                .usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
            });

            pointLightGrid = coffee::graphics::Image::create(device, {
                .imageType = VK_IMAGE_TYPE_2D,
                .format = VK_FORMAT_R32G32_UINT,
                .extent = { .width = widthInTiles, .height = heightInTiles },
                .usage = VK_IMAGE_USAGE_STORAGE_BIT
            });

            pointLightGridView = coffee::graphics::ImageView::create(pointLightGrid, {
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = VK_FORMAT_R32G32_UINT
            });

            spotLightGrid = coffee::graphics::Image::create(device, {
                .imageType = VK_IMAGE_TYPE_2D,
                .format = VK_FORMAT_R32G32_UINT,
                .extent = { .width = widthInTiles, .height = heightInTiles },
                .usage = VK_IMAGE_USAGE_STORAGE_BIT
            });

            spotLightGridView = coffee::graphics::ImageView::create(spotLightGrid, {
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = VK_FORMAT_R32G32_UINT
            });

            layoutBarriers[index * coffee::graphics::Device::kMaxOperationsInFlight + 0] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = pointLightGrid->image(),
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            layoutBarriers[index * coffee::graphics::Device::kMaxOperationsInFlight + 1] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = spotLightGrid->image(),
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };
        }

        auto layoutTranslationCommandBuffer = coffee::graphics::CommandBuffer::createCompute(device);

        layoutTranslationCommandBuffer.imagePipelineBarrier(
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            layoutBarriers.size(),
            layoutBarriers.data()
        );

        device->submit(std::move(layoutTranslationCommandBuffer), {}, layoutTranslationFence, true);

        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            auto& lightCullingDescriptor = lightCullingDescriptors[index];
            auto writer = coffee::graphics::DescriptorWriter(lightCullingLayout);

            if (lightCullingDescriptor == nullptr) {
                writer.addImage (0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, earlyDepth.view, depthSampler);
                writer.addBuffer(1, calculatedFrustums);
                writer.addBuffer(2, pointLightsBuffers[index]);
                writer.addBuffer(3, spotLightsBuffers[index]);
                writer.addBuffer(4, indexCounters[index]);
                writer.addBuffer(5, pointLightIndexLists[index]);
                writer.addBuffer(6, spotLightIndexLists[index]);
                writer.addImage (7, VK_IMAGE_LAYOUT_GENERAL, pointLightGridViews[index]);
                writer.addImage (8, VK_IMAGE_LAYOUT_GENERAL, spotLightGridViews[index]);

                lightCullingDescriptor = coffee::graphics::DescriptorSet::create(device, writer);
            }
            else {
                writer.addImage (0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, earlyDepth.view, depthSampler);
                writer.addBuffer(5, pointLightIndexLists[index]);
                writer.addBuffer(6, spotLightIndexLists[index]);
                writer.addImage (7, VK_IMAGE_LAYOUT_GENERAL, pointLightGridViews[index]);
                writer.addImage (8, VK_IMAGE_LAYOUT_GENERAL, spotLightGridViews[index]);

                lightCullingDescriptor->update(writer);
            }
        }
    }

    // clang-format on

    void LightCulling::reset(const coffee::graphics::CommandBuffer& commandBuffer) const noexcept
    {
        constexpr VkBufferCopy indexCleanupCopyRegion { .srcOffset = 0, .dstOffset = 0, .size = sizeof(OutputIndexCounters) };
        commandBuffer.copyBuffer(defaultIndexCounters, indexCounters[frameIndex], 1, &indexCleanupCopyRegion);
    }

    void LightCulling::perform(const coffee::graphics::CommandBuffer& commandBuffer, const glm::mat4& viewMatrix) const noexcept
    {
        COFFEE_ASSERT(calculatedFrustums != nullptr, "Did you forgot to call 'resize' before using LightCulling?");

        LightCullingPushConstants pushConstants {};
        pushConstants.inverseProjection = inverseProjection;
        pushConstants.viewMatrix = viewMatrix;

        commandBuffer.bindPipeline(lightCullingPipeline);
        commandBuffer.bindDescriptorSets(lightCullingPipeline, lightCullingDescriptors[frameIndex]);
        commandBuffer.pushConstants(lightCullingPipeline, sizeof(pushConstants), &pushConstants);
        commandBuffer.dispatch(widthInTiles, heightInTiles, 1);
    }

    void LightCulling::nextFrame() noexcept { frameIndex = (frameIndex + 1) % coffee::graphics::Device::kMaxOperationsInFlight; }

    uint32_t LightCulling::currentFrame() const noexcept { return frameIndex; }

} // namespace yorna