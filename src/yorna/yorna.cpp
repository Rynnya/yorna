#include <yorna/yorna.hpp>

#include <coffee/coffee.hpp>
#include <coffee/objects/vertex.hpp>
#include <coffee/utils/utils.hpp>

#include <imgui.h>

namespace yorna {

    static constexpr std::array<VkClearValue, 2> normalClearValues = { VkClearValue { .color = { 0.0f, 0.0f, 0.0f, 1.0f } },
                                                                       VkClearValue { .depthStencil = { 0.0f, 0U } } };

    static constexpr std::array<VkClearValue, 1> depthClearValues = { VkClearValue { .depthStencil = { 0.0f, 0U } } };

    Yorna::Yorna(const coffee::graphics::DevicePtr& device, coffee::LoopHandler& loopHandler)
        : device { device }
        , loopHandler { loopHandler }
        , assetManager { coffee::AssetManager::create(device) }
        , filesystem { coffee::Filesystem::create("amazon.fs") }
        , sunlightShadow { device, assetManager, filesystem, 2048U }
    {
        createSamplers();
        createBuffers();
        createRenderPasses();
        loadModels();
        createImages();
        createDescriptors();
        createFramebuffer();
        createPipelines();

        for (auto& frameInfo : frameInfos) {
            frameInfo.mvpUbo.projection = camera.projection();
            frameInfo.mvpUbo.view = camera.view();
            frameInfo.mvpUbo.inverseView = camera.inverseView();

            std::memcpy(frameInfo.mvpBuffer->memory(), &frameInfo.mvpUbo, sizeof(frameInfo.mvpUbo));
            frameInfo.mvpBuffer->flush();
        }
    }

    Yorna::~Yorna() { device->waitForRelease(); }

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
        cullMeshes();
        updateObjects();
        updateLightPoints();
    }

    void Yorna::cullMeshes()
    {
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

    void Yorna::performDepthTest(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        const VkRect2D renderArea = {
            .extent = {
                .width = depthImage->extent.width,
                .height = depthImage->extent.height,
            }
        };

        const VkViewport viewport = {
            .width = static_cast<float>(renderArea.extent.width),
            .height = static_cast<float>(renderArea.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        auto& meshes = model->model->meshes;
        auto& visibleMeshes = model->visibleMeshes;
        auto& frameInfo = frameInfos[frameInfoIndex];
        auto transformationMatrix = model->transform.mat4();

        commandBuffer.beginRenderPass(earlyDepthPass, earlyDepthFramebuffer, renderArea, depthClearValues.size(), depthClearValues.data());
        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);

        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, earlyDepthPipeline);
        commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, earlyDepthPipeline, frameInfo.descriptorSet);
        commandBuffer.pushConstants(earlyDepthPipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(transformationMatrix), &transformationMatrix);
        commandBuffer.bindModel(model->model);

        for (size_t meshID : visibleMeshes) {
            commandBuffer.drawMesh(meshes[meshID]);
        }

        commandBuffer.endRenderPass();
    }

    void Yorna::performRendering(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        auto& meshes = model->model->meshes;
        auto& visibleMeshes = model->visibleMeshes;

        auto& frameInfo = frameInfos[frameInfoIndex];
        auto& currentLightBuffer = frameInfo.lightUbo;
        auto transformationMatrix = model->transform.mat4();

        SunLightShadow::PushConstants shadowConstants {
            .lightSpaceMatrix = frameInfos[frameInfoIndex].lightUbo.sunlightSpaceMatrix,
            .modelMatrix = transformationMatrix,
        };

        sunlightShadow.beginRender(commandBuffer);
        commandBuffer.pushConstants(sunlightShadow.pipeline(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(shadowConstants), &shadowConstants);
        commandBuffer.bindModel(model->model);
        commandBuffer.drawModel(model->model);
        sunlightShadow.endRender(commandBuffer);

        const VkRect2D renderArea = {
            .extent = {
                .width = depthImage->extent.width,
                .height = depthImage->extent.height,
            }
        };

        const VkViewport viewport = {
            .width = static_cast<float>(renderArea.extent.width),
            .height = static_cast<float>(renderArea.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        commandBuffer.beginRenderPass(renderPass, framebuffer, renderArea, normalClearValues.size(), normalClearValues.data());
        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline);
        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);

        mainConstants.transform = transformationMatrix;
        mainConstants.normal = model->transform.normal();
        commandBuffer.pushConstants(mainPipeline, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(mainConstants), &mainConstants);
        commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline, frameInfo.descriptorSet);
        commandBuffer.bindModel(model->model);

        for (size_t meshID : visibleMeshes) {
            commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline, model->descriptors[meshID], 1);
            commandBuffer.drawMesh(meshes[meshID]);
        }

        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, lightPointsPipeline);
        commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, lightPointsPipeline, frameInfo.descriptorSet);

        for (uint32_t i = 0; i < currentLightBuffer.amountOfPointLights; i++) {
            lightPointsConstants.position = currentLightBuffer.pointLights[i].position;
            lightPointsConstants.color = currentLightBuffer.pointLights[i].color;

            commandBuffer.pushConstants(
                lightPointsPipeline,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                sizeof(lightPointsConstants),
                &lightPointsConstants
            );
            commandBuffer.draw(6);
        }

        for (uint32_t i = 0; i < currentLightBuffer.amountOfSpotLights; i++) {
            lightPointsConstants.position = currentLightBuffer.spotLights[i].position;
            lightPointsConstants.color = currentLightBuffer.spotLights[i].color;

            commandBuffer.pushConstants(
                lightPointsPipeline,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                sizeof(lightPointsConstants),
                &lightPointsConstants
            );
            commandBuffer.draw(6);
        }

        commandBuffer.endRenderPass();
        frameInfoIndex = (frameInfoIndex + 1) % coffee::graphics::Device::kMaxOperationsInFlight;
    }

    void Yorna::updateCamera()
    {
        camera.setViewYXZ(viewerObject.translation, viewerObject.rotation);
        camera.setReversePerspectiveProjection(glm::radians(80.0f), outputAspect.load(std::memory_order_relaxed));
        camera.recalculateFrustumPlanes();

        auto& frameInfo = frameInfos[frameInfoIndex];
        frameInfo.mvpUbo.projection = camera.projection();
        frameInfo.mvpUbo.view = camera.view();
        frameInfo.mvpUbo.inverseView = camera.inverseView();

        std::memcpy(frameInfo.mvpBuffer->memory(), &frameInfo.mvpUbo, sizeof(frameInfo.mvpUbo));
        frameInfo.mvpBuffer->flush();
    }

    void Yorna::updateObjects()
    {
        if (boundWindow == nullptr) {
            updateCamera();
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

        updateCamera();
    }

    void Yorna::updateLightPoints()
    {
        auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, loopHandler.deltaTime(), { 0.0f, -1.0f, 0.0f });
        auto& lightUbo = frameInfos[frameInfoIndex].lightUbo;

        lightUbo.sunlightSpaceMatrix = sunlightShadow.camera().projection() * sunlightShadow.camera().view();
        lightUbo.sunlightDirection = glm::vec4 { sunlightShadow.lightObject().direction, 1.0f };
        lightUbo.sunlightColor = glm::vec4 { sunlightShadow.lightObject().color, 1.0f };

        lightUbo.amountOfPointLights = static_cast<uint32_t>(pointLights.size());
        lightUbo.amountOfSpotLights = static_cast<uint32_t>(spotLights.size());

        for (size_t i = 0; i < pointLights.size(); i++) {
            pointLights[i].position = glm::vec3(rotateLight * glm::vec4 { pointLights[i].position, 1.0f });

            lightUbo.pointLights[i].position = glm::vec4(pointLights[i].position, 0.1f);
            lightUbo.pointLights[i].color = glm::vec4(pointLights[i].color, pointLights[i].intensity);
        }

        for (size_t i = 0; i < spotLights.size(); i++) {
            lightUbo.spotLights[i].position = glm::vec4(spotLights[i].position, 0.1f);
            lightUbo.spotLights[i].color = glm::vec4(spotLights[i].color, spotLights[i].intensity);
            lightUbo.spotLights[i].coneDirection = glm::vec4(spotLights[i].coneDirection, spotLights[i].coneAngle);
        }

        std::memcpy(frameInfos[frameInfoIndex].lightBuffer->memory(), &lightUbo, sizeof(lightUbo));
        frameInfos[frameInfoIndex].lightBuffer->flush();
    }

    // clang-format off

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

    void Yorna::createBuffers()
    {
        for (auto& frameInfo : frameInfos) {
            frameInfo.mvpBuffer = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(MVPUniformBuffer),
                .instanceCount = 1U,
                .usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            });

            frameInfo.lightBuffer = coffee::graphics::Buffer::create(device, {
                .instanceSize = sizeof(LightUniformBuffer),
                .instanceCount = 1U,
                .usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            });
        }
    }

    void Yorna::loadModels()
    {
        model = std::make_unique<Model>(device, assetManager->getModel(filesystem, "exterior.cfa"), textureSampler);

        viewerObject.translation.z = -1.5f;
        viewerObject.translation.y = 1.0f;
    }

    void Yorna::createRenderPasses()
    {
        earlyDepthPass = coffee::graphics::RenderPass::create(device, {
            .depthStencilAttachment = coffee::graphics::AttachmentConfiguration {
                .format = device->optimalDepthFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            },
        });

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
            .depthStencilAttachment = coffee::graphics::AttachmentConfiguration {
                .format = device->optimalDepthFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            },
        });
    }

    void Yorna::createPipelines()
    {
        earlyDepthPipeline = coffee::graphics::Pipeline::create(device, earlyDepthPass, {
            .vertexShader = assetManager->getShader(filesystem, "shaders/early_depth.vert.spv"),
            .layouts = {
                gameLayout
            },
            .inputBindings = { coffee::graphics::InputBinding {
                .binding = 0U,
                .stride = sizeof(coffee::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = coffee::Vertex::getElementDescriptions()
            }},
            .constantRanges = { coffee::graphics::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT,
                .size = static_cast<uint32_t>(sizeof(glm::mat4))
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_BACK_BIT
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL
            }
        });

        mainPipeline = coffee::graphics::Pipeline::create(device, renderPass, {
            .vertexShader = assetManager->getShader(filesystem, "shaders/forward.vert.spv"),
            .fragmentShader = assetManager->getShader(filesystem, "shaders/forward.frag.spv"),
            .layouts = {
                gameLayout,
                model->layout
            },
            .inputBindings = { coffee::graphics::InputBinding {
                .binding = 0U,
                .stride = sizeof(coffee::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = coffee::Vertex::getElementDescriptions()
            }},
            .constantRanges = { coffee::graphics::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .size = static_cast<uint32_t>(sizeof(MainPushConstants))
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_BACK_BIT
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_EQUAL
            },
        });

        lightPointsPipeline = coffee::graphics::Pipeline::create(device, renderPass, {
            .vertexShader = assetManager->getShader(filesystem, "shaders/point_light.vert.spv"),
            .fragmentShader = assetManager->getShader(filesystem, "shaders/point_light.frag.spv"),
            .layouts = {
                gameLayout
            },
            .constantRanges = { coffee::graphics::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .size = static_cast<uint32_t>(sizeof(LightPushConstants))
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_BACK_BIT
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL
            },
        });
    }

    void Yorna::createImages()
    {
        colorImage = coffee::graphics::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->surfaceFormat(),
            .extent = { 1920U, 1080U, },
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .priority = 1.0f
        });

        colorImageView = coffee::graphics::ImageView::create(colorImage, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = device->surfaceFormat()
        });

        depthImage = coffee::graphics::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->optimalDepthFormat(),
            .extent = { 1920U, 1080U, },
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .priority = 1.0f
        });

        depthImageView = coffee::graphics::ImageView::create(depthImage, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = device->optimalDepthFormat(),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        });
    }

    void Yorna::createFramebuffer()
    {
        earlyDepthFramebuffer = coffee::graphics::Framebuffer::create(device, earlyDepthPass, {
            .extent = { depthImage->extent.width, depthImage->extent.height },
            .depthStencilView = depthImageView
        });

        framebuffer = coffee::graphics::Framebuffer::create(device, renderPass, {
            .extent = { colorImage->extent.width, colorImage->extent.height },
            .colorViews = { colorImageView },
            .depthStencilView = depthImageView
        });
    }

    // clang-format on

    void Yorna::updateDescriptors()
    {
        coffee::graphics::DescriptorWriter writer = coffee::graphics::DescriptorWriter(outputLayout);

        outputSet->updateDescriptor(writer.addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, colorImageView, textureSampler));
    }

    void Yorna::createDescriptors()
    {
        // clang-format off
        gameLayout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 1, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 2, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT } },
        });

        outputLayout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT } },
        });
        // clang-format on

        for (auto& frameInfo : frameInfos) {
            coffee::graphics::DescriptorWriter writer = coffee::graphics::DescriptorWriter(gameLayout);

            writer.addBuffer(0, frameInfo.mvpBuffer);
            writer.addBuffer(1, frameInfo.lightBuffer);
            writer.addImage(2, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, sunlightShadow.mapView(), sunlightShadow.sampler());

            frameInfo.descriptorSet = coffee::graphics::DescriptorSet::create(device, writer);
        }

        coffee::graphics::DescriptorWriter outputWriter = coffee::graphics::DescriptorWriter(outputLayout);
        outputWriter.addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, colorImageView, textureSampler);

        outputSet = coffee::graphics::DescriptorSet::create(device, outputWriter);
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