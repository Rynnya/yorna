#include <yorna/yorna.hpp>

#include <coffee/coffee.hpp>
#include <coffee/objects/vertex.hpp>
#include <coffee/utils/utils.hpp>

#include <imgui.h>

#include <glm/gtc/random.hpp>

namespace yorna {

    Yorna::Yorna(const SharedInstance& instance, coffee::LoopHandler& loopHandler)
        : SharedInstance { instance }
        , loopHandler { loopHandler }
        , earlyDepth { instance, outputViewport.x, outputViewport.y }
        , sunlightShadow { instance, 2048U }
        , lightCulling { instance, earlyDepth, kMaxAmountOfPointLights, kMaxAmountOfSpotLights }
        , forwardPlus { instance, earlyDepth, sunlightShadow, lightCulling }
    {
        camera.setViewYXZ(viewerObject.translation, viewerObject.rotation);
        camera.setReversePerspectiveProjection(80.0f, outputAspect.load(std::memory_order_relaxed));
        camera.recalculateFrustumPlanes();

        lightCulling.resize(camera, outputViewport.x, outputViewport.y);
        forwardPlus.resize(outputViewport.x, outputViewport.y);

        createBuffers();
        createSamplers();
        createDescriptors();
        createSyncObjects();
        loadModels();

        std::random_device rngDevice {};
        std::mt19937_64 rngEngine { rngDevice() };

        std::uniform_real_distribution<float> positionDist { -15.0f, 15.0f };
        std::uniform_real_distribution<float> heightDist { -3.0f, 12.0f };
        std::uniform_real_distribution<float> colorDist { 0.3f, 0.8f };

        auto* pointLightsArray = pointLights->memory<PointLight*>();

        for (size_t index = 0; index < kMaxAmountOfPointLights; index++) {
            pointLightsArray[index] = {
                .position = { positionDist(rngEngine), heightDist(rngEngine), positionDist(rngEngine) },
                .radius = 2.0f,
                .color = { colorDist(rngEngine), colorDist(rngEngine), colorDist(rngEngine) },
            };
        }
    }

    Yorna::~Yorna() { device->waitDeviceIdle(); }

    void Yorna::bindWindow(const coffee::graphics::Window* window) noexcept
    {
        boundWindow = window;

        if (boundWindow != nullptr) {
            boundWindow->mouseMoveEvent += { this, &Yorna::mousePositionCallback };
            boundWindow->keyEvent += { this, &Yorna::keyCallback };
        }
    }

    void Yorna::update()
    {
        auto commandBuffer = coffee::graphics::CommandBuffer::createTransfer(device);

        updateObjects(commandBuffer);
        updateLights(commandBuffer);
        cullMeshes();

        completionFences[frameIndex]->wait();
        completionFences[frameIndex]->reset();

        device->submit(std::move(commandBuffer), submitUpdateUBOSemaphores[frameIndex]);
    }

    void Yorna::cullMeshes()
    {
        for (auto& model : models) {
            auto& meshes = model->model->meshes;
            auto& visibleMeshes = model->visibleMeshes;
            auto transformationMatrix = model->transform.mat4();

            visibleMeshes.clear();

            for (size_t i = 0; i < meshes.size(); i++) {
                if (camera.isInFrustum(transformationMatrix, meshes[i].aabb)) {
                    visibleMeshes.push_back(i);
                }
            }
        }
    }

    void Yorna::performDepthTest(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        sunlightShadow.begin(commandBuffer);

        for (auto& model : models) {
            auto& meshes = model->model->meshes;
            auto& visibleMeshes = model->visibleMeshes;

            sunlightShadow.push(commandBuffer, model->transform.mat4());
            commandBuffer.bindModel(model->model);
            commandBuffer.drawModel(model->model);
        }

        sunlightShadow.end(commandBuffer);

        earlyDepth.begin(commandBuffer);

        for (auto& model : models) {
            auto& meshes = model->model->meshes;
            auto& visibleMeshes = model->visibleMeshes;

            earlyDepth.push(commandBuffer, model->transform.mat4());
            commandBuffer.bindModel(model->model);

            for (size_t meshID : visibleMeshes) {
                commandBuffer.drawMesh(meshes[meshID]);
            }
        }

        earlyDepth.end(commandBuffer);

        if (!device->isUnifiedGraphicsComputeQueue()) {
            VkImageSubresourceRange depthResource {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            VkImageMemoryBarrier releaseDepthBarrier {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = 0,
                .oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .image = earlyDepth.image->image(),
                .subresourceRange = depthResource
            };

            commandBuffer.imagePipelineBarrier(
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0,
                1,
                &releaseDepthBarrier
            );
        }
    }

    void Yorna::submitDepthTest(coffee::graphics::CommandBuffer&& commandBuffer)
    {
        device->submit(std::move(commandBuffer), submitEarlyDepthSemaphores[frameIndex]);
    }

    void Yorna::performLightCulling(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        if (!device->isUnifiedGraphicsComputeQueue()) {
            VkImageSubresourceRange depthResource {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            VkImageMemoryBarrier acquireDepthBarrier {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .image = earlyDepth.image->image(),
                .subresourceRange = depthResource
            };

            commandBuffer.imagePipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1, &acquireDepthBarrier);
        }

        lightCulling.perform(commandBuffer, camera.view());

        if (!device->isUnifiedGraphicsComputeQueue()) {
            VkImageSubresourceRange depthResource {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            VkImageSubresourceRange colorResource {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            std::array<VkImageMemoryBarrier, 3> releaseImageBarriers {};

            releaseImageBarriers[0] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .dstAccessMask = 0,
                .oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .image = earlyDepth.image->image(),
                .subresourceRange = depthResource
            };

            releaseImageBarriers[1] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
                .dstAccessMask = 0,
                .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
                .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .image = lightCulling.pointLightGrid->image(),
                .subresourceRange = colorResource
            };

            releaseImageBarriers[2] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
                .dstAccessMask = 0,
                .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
                .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .image = lightCulling.spotLightGrid->image(),
                .subresourceRange = colorResource
            };

            commandBuffer.imagePipelineBarrier(
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0,
                releaseImageBarriers.size(),
                releaseImageBarriers.data()
            );
        }
    }

    void Yorna::submitLightCulling(coffee::graphics::CommandBuffer&& commandBuffer)
    {
        device->submit(std::move(commandBuffer), submitLightCullingSemaphores[frameIndex]);
    }

    void Yorna::performRendering(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        if (!device->isUnifiedGraphicsComputeQueue()) {
            VkImageSubresourceRange depthResource {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            VkImageSubresourceRange colorResource {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            std::array<VkImageMemoryBarrier, 3> acquireImageBarriers {};

            acquireImageBarriers[0] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .image = earlyDepth.image->image(),
                .subresourceRange = depthResource
            };

            acquireImageBarriers[1] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
                .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .image = lightCulling.pointLightGrid->image(),
                .subresourceRange = colorResource
            };

            acquireImageBarriers[2] = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
                .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                .srcQueueFamilyIndex = device->computeQueueFamilyIndex(),
                .dstQueueFamilyIndex = device->graphicsQueueFamilyIndex(),
                .image = lightCulling.spotLightGrid->image(),
                .subresourceRange = colorResource
            };

            commandBuffer.imagePipelineBarrier(
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
                0,
                acquireImageBarriers.size(),
                acquireImageBarriers.data()
            );
        }

        forwardPlus.begin(commandBuffer);

        for (auto& model : models) {
            auto& meshes = model->model->meshes;
            auto& visibleMeshes = model->visibleMeshes;
            auto transformationMatrix = model->transform.mat4();

            forwardPlus.push(commandBuffer, transformationMatrix, model->transform.normal());
            commandBuffer.bindModel(model->model);

            for (size_t meshID : visibleMeshes) {
                forwardPlus.bind(commandBuffer, model->descriptors[meshID]);
                commandBuffer.drawMesh(meshes[meshID]);
            }
        }

        forwardPlus.end(commandBuffer);
    }

    void Yorna::submitRendering(coffee::graphics::CommandBuffer&& commandBuffer)
    {
        device->submit(std::move(commandBuffer), submitRenderingSemaphores[frameIndex], completionFences[frameIndex]);
    }

    void Yorna::updateCamera(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        camera.setViewYXZ(viewerObject.translation, viewerObject.rotation);
        camera.setReversePerspectiveProjection(80.0f, outputAspect.load(std::memory_order_relaxed));
        camera.recalculateFrustumPlanes();

        auto& ubo = mvpUniformBuffers[frameIndex]->memory<MVPUniformBuffer>();
        ubo.projection = camera.projection();
        ubo.view = camera.view();
        ubo.inverseView = camera.inverseView();
    }

    void Yorna::updateObjects(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        if (boundWindow == nullptr) {
            updateCamera(commandBuffer);
            return;
        }

        const float yaw = viewerObject.rotation.y;

        const glm::vec3 forwardDir { glm::sin(yaw), 0.0f, glm::cos(yaw) };
        const glm::vec3 rightDir { forwardDir.z, 0.0f, -forwardDir.x };
        const glm::vec3 upDir { 0.0f, 1.0f, 0.0f };

        glm::vec3 moveDir { 0.0f };

        if (boundWindow->isButtonPressed(coffee::Keys::S)) {
            moveDir -= forwardDir;
        }

        if (boundWindow->isButtonPressed(coffee::Keys::W)) {
            moveDir += forwardDir;
        }

        if (boundWindow->isButtonPressed(coffee::Keys::A)) {
            moveDir -= rightDir;
        }

        if (boundWindow->isButtonPressed(coffee::Keys::D)) {
            moveDir += rightDir;
        }

        if (boundWindow->isButtonPressed(coffee::Keys::LeftShift)) {
            moveDir -= upDir;
        }

        if (boundWindow->isButtonPressed(coffee::Keys::Space)) {
            moveDir += upDir;
        }

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            viewerObject.translation += moveSpeed * loopHandler.deltaTime() * glm::normalize(moveDir);
        }

        updateCamera(commandBuffer);
    }

    void Yorna::updateLights(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        auto* pointLightsArray = pointLights->memory<PointLight*>();

        for (size_t index = 0; index < kMaxAmountOfPointLights; index++) {
            pointLightsArray[index].position.y += 1.4f * loopHandler.deltaTime();

            if (pointLightsArray[index].position.y > 12.0f) {
                pointLightsArray[index].position.y -= 12.0f;
            }
        }

        constexpr VkBufferCopy pointLightCopyRegion { .srcOffset = 0, .dstOffset = 0, .size = kMaxAmountOfPointLights * sizeof(PointLight) };
        commandBuffer.copyBuffer(pointLights, lightCulling.pointLightsBuffers[lightCulling.currentFrame()], 1, &pointLightCopyRegion);

        constexpr VkBufferCopy spotLightCopyRegion { .srcOffset = 0, .dstOffset = 0, .size = kMaxAmountOfSpotLights * sizeof(SpotLight) };
        commandBuffer.copyBuffer(spotLights, lightCulling.spotLightsBuffers[lightCulling.currentFrame()], 1, &spotLightCopyRegion);

        lightCulling.reset(commandBuffer);
    }

    void Yorna::nextFrame() noexcept
    {
        earlyDepth.nextFrame();
        sunlightShadow.nextFrame();
        lightCulling.nextFrame();
        forwardPlus.nextFrame();

        frameIndex = (frameIndex + 1) % coffee::graphics::Device::kMaxOperationsInFlight;
    }

    uint32_t Yorna::currentFrame() const noexcept { return frameIndex; }

    // clang-format off

    void Yorna::createBuffers()
    {
        pointLights = coffee::graphics::Buffer::create(device, {
            .instanceSize = static_cast<uint32_t>(sizeof(PointLight)),
            .instanceCount = kMaxAmountOfPointLights,
            .usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST
        });

        spotLights = coffee::graphics::Buffer::create(device, {
            .instanceSize = static_cast<uint32_t>(sizeof(SpotLight)),
            .instanceCount = kMaxAmountOfSpotLights,
            .usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST
        });

        std::memset(pointLights->memory(), 0, kMaxAmountOfPointLights * sizeof(PointLight));
        std::memset(spotLights->memory(), 0, kMaxAmountOfSpotLights * sizeof(SpotLight));
    }

    void Yorna::createSamplers()
    {
        textureSampler = coffee::graphics::Sampler::create(device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .anisotropyEnable = true,
            .maxAnisotropy = 256U, // Set to huge value that will be rounded down to device maximum
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = VK_LOD_CLAMP_NONE,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        });

        outputSampler = coffee::graphics::Sampler::create(device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .minLod = 0.0f,
            .maxLod = 1.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        });
    }

    void Yorna::createDescriptors()
    {
        outputLayout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT } }
        });

        coffee::graphics::DescriptorWriter outputWriter = coffee::graphics::DescriptorWriter(outputLayout);
        outputWriter.addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, forwardPlus.view, textureSampler);

        outputDescriptor = coffee::graphics::DescriptorSet::create(device, outputWriter);
    }

    void Yorna::createSyncObjects()
    {
        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            updateUBOSemaphores[index] = coffee::graphics::Semaphore::create(device);
            earlyDepthSemaphores[index] = coffee::graphics::Semaphore::create(device);
            lightCullingSemaphores[index] = coffee::graphics::Semaphore::create(device);
            completionFences[index] = coffee::graphics::Fence::create(device, true);

            submitUpdateUBOSemaphores[index].signalSemaphores.push_back(updateUBOSemaphores[index]);
            submitEarlyDepthSemaphores[index].signalSemaphores.push_back(earlyDepthSemaphores[index]);

            submitLightCullingSemaphores[index].waitSemaphores.push_back(updateUBOSemaphores[index]);
            submitLightCullingSemaphores[index].waitDstStageMasks.push_back(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            submitLightCullingSemaphores[index].waitSemaphores.push_back(earlyDepthSemaphores[index]);
            submitLightCullingSemaphores[index].waitDstStageMasks.push_back(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            submitLightCullingSemaphores[index].signalSemaphores.push_back(lightCullingSemaphores[index]);

            submitRenderingSemaphores[index].waitSemaphores.push_back(lightCullingSemaphores[index]);
            submitRenderingSemaphores[index].waitDstStageMasks.push_back(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        }
    }

    void Yorna::loadModels()
    {
        models.push_back(std::make_unique<Model>(device, assetManager->getModel(filesystem, "sponza_scene.cfa"), textureSampler));

        device->waitDeviceIdle();

        viewerObject.translation.z = -1.5f;
        viewerObject.translation.y = 1.0f;
    }

    // clang-format on

    void Yorna::updateDescriptors()
    {
        coffee::graphics::DescriptorWriter writer = coffee::graphics::DescriptorWriter(outputLayout);
        writer.addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, forwardPlus.view, textureSampler);

        outputDescriptor->update(writer);
    }

    void Yorna::mousePositionCallback(const coffee::graphics::Window& window, const coffee::MouseMoveEvent& e) noexcept
    {
        if (window.cursorState() != coffee::graphics::CursorState::Disabled) {
            return;
        }

        coffee::Float2D mousePosition = window.mousePosition();
        viewerObject.rotation.x = glm::clamp(viewerObject.rotation.x + lookSpeed * (e.y - mousePosition.y), -1.5f, 1.5f);
        viewerObject.rotation.y = glm::mod(viewerObject.rotation.y + lookSpeed * (e.x - mousePosition.x), glm::two_pi<float>());
    }

    void Yorna::keyCallback(const coffee::graphics::Window& window, const coffee::KeyEvent& e) noexcept
    {
        switch (e.key) {
            case coffee::Keys::Escape:
                window.showCursor();
                break;
            case coffee::Keys::Enter:
                window.showCursor();
                break;
            case coffee::Keys::W:
            case coffee::Keys::A:
            case coffee::Keys::S:
            case coffee::Keys::D:
            default:
                break;
        }
    };

} // namespace yorna