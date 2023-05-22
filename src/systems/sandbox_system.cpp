#include <systems/sandbox/system.hpp>

#include <coffee/coffee.hpp>
#include <coffee/objects/vertex.hpp>
#include <coffee/utils/utils.hpp>

#include <imgui.h>

namespace editor {

    static constexpr std::array<VkClearValue, 2> normalClearValues = {
        VkClearValue { .color = { 0.0f, 0.0f, 0.0f, 1.0f } },
        VkClearValue { .depthStencil = { 0.0f, 0U } }
    };

    static constexpr std::array<VkClearValue, 1> depthClearValues = {
        VkClearValue { .depthStencil = { 0.0f, 0U } }
    };

    std::vector<glm::vec4> lightColors {
        {1.0f, 0.1f, 0.1f, 3.0f},
        {0.1f, 0.1f, 1.0f, 3.0f},
        {0.1f, 1.0f, 0.1f, 3.0f},
        {1.0f, 1.0f, 0.1f, 3.0f},
        {0.1f, 1.0f, 1.0f, 3.0f},
        {1.0f, 1.0f, 1.0f, 3.0f}
    };

    MainSystem::MainSystem(const coffee::GPUDevicePtr& device, coffee::LoopHandler& loopHandler)
        : device { device }
        , loopHandler { loopHandler }
        , assetManager { coffee::AssetManager::create(device) }
        , filesystem { coffee::Filesystem::create("nativefs/") }
    {
        frameInfos.resize(device->imageCount());

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

        for (size_t i = 0; i < lightColors.size(); i++) {
            PointLight& object = pointLights.emplace_back(lightColors[i].a, 0.1f, lightColors[i]);
            auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, (i * glm::two_pi<float>()) / lightColors.size(), { 0.0f, -1.0f, 0.0f });
            object.transform.translation = glm::vec3(rotateLight * glm::vec4 { -0.5f, 1.0f, -0.5f, 1.0f });
        }

        //window.addWindowResizeCallback("main", [this](const coffee::Window&, const coffee::ResizeEvent&) {
        //    createImages();
        //    createFramebuffer();
        //    updateDescriptors();
        //});

        //window.addMouseClickCallback("main", [](const coffee::Window& window, const coffee::MouseClickEvent& e) {
        //    if (e.button == coffee::MouseButton::Right) {
        //        window.disableCursor();
        //        window.disableTextMode();
        //    }
        //});
    }

    MainSystem::~MainSystem() {
        vkDeviceWaitIdle(device->logicalDevice());
    }

    void MainSystem::bindWindow(const coffee::Window* window) noexcept {
        boundWindow = window;

        boundWindow->mouseMoveEvent +=
            coffee::Callback<const coffee::Window&, const coffee::MouseMoveEvent&> { this, & MainSystem::mousePositionCallback };
        boundWindow->keyEvent +=
            coffee::Callback<const coffee::Window&, const coffee::KeyEvent&> { this, & MainSystem::keyCallback };
    }

    void MainSystem::unbindWindow() noexcept {
        if (boundWindow != nullptr) {
            boundWindow->mouseMoveEvent -=
                coffee::Callback<const coffee::Window&, const coffee::MouseMoveEvent&> { this, & MainSystem::mousePositionCallback };
            boundWindow->keyEvent -=
                coffee::Callback<const coffee::Window&, const coffee::KeyEvent&> { this, & MainSystem::keyCallback };

            boundWindow = nullptr;
        }
    }

    void MainSystem::update() {
        updateObjects();
        updateLightPoints();
    }

    void MainSystem::performDepthTest(const coffee::CommandBuffer& commandBuffer) {
        const VkRect2D renderArea = {
            .extent = {
                .width = depthImage->extent.width, 
                .height = depthImage->extent.height
            }
        };

        const VkViewport viewport = {
            .width = static_cast<float>(renderArea.extent.width),
            .height = static_cast<float>(renderArea.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        const auto sponzaMat4 = sponzaModel->transform.mat4();
        const auto& frameInfo = frameInfos[frameInfoIndex];

        commandBuffer.beginRenderPass(earlyDepthPass, earlyDepthFramebuffer, renderArea, depthClearValues.size(), depthClearValues.data());
        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);

        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, earlyDepthPipeline);
        commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, earlyDepthPipeline, 1, &frameInfo.descriptorSet->set());
        commandBuffer.pushConstants(earlyDepthPipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(sponzaMat4), &sponzaMat4);
        sponzaModel->model->bind(commandBuffer);
        sponzaModel->model->draw(commandBuffer);

        commandBuffer.endRenderPass();
    }

    void MainSystem::performRendering(const coffee::CommandBuffer& commandBuffer) {
        const VkRect2D renderArea = {
            .extent = {
                .width = depthImage->extent.width,
                .height = depthImage->extent.height 
            }
        };

        const VkViewport viewport = {
            .width = static_cast<float>(renderArea.extent.width),
            .height = static_cast<float>(renderArea.extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        const auto sponzaMat4 = sponzaModel->transform.mat4();
        const auto& meshes = sponzaModel->model->meshes;
        const auto& frameInfo = frameInfos[frameInfoIndex];
        const auto& currentLightBuffer = frameInfo.lightUbo;

        commandBuffer.beginRenderPass(renderPass, framebuffer, renderArea, normalClearValues.size(), normalClearValues.data());
        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline);
        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);

        mainConstants.transform = sponzaMat4;
        mainConstants.normal = sponzaModel->transform.normal();
        commandBuffer.pushConstants(
            mainPipeline,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            sizeof(mainConstants),
            &mainConstants);

        std::array<VkDescriptorSet, 2> sets {};
        sets[0] = frameInfo.descriptorSet->set();
        sponzaModel->model->bind(commandBuffer);

        for (size_t i = 0; i < meshes.size(); i++) {
            sets[1] = sponzaModel->descriptors[i]->set();
            commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mainPipeline, sets.size(), sets.data());

            meshes[i].draw(commandBuffer);
        }

        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, lightPointsPipeline);
        commandBuffer.setViewport(viewport);
        commandBuffer.setScissor(renderArea);
        commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, lightPointsPipeline, 1, &frameInfo.descriptorSet->set());

        for (uint32_t i = 0; i < currentLightBuffer.size; i++) {
            lightPointsConstants.position = currentLightBuffer.lightPoints[i].position;
            lightPointsConstants.color = currentLightBuffer.lightPoints[i].color;

            commandBuffer.pushConstants(
                lightPointsPipeline,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                sizeof(lightPointsConstants),
                &lightPointsConstants);
            commandBuffer.draw(6);
        }

        commandBuffer.endRenderPass();
        frameInfoIndex = (frameInfoIndex + 1) % device->imageCount();
    }

    void MainSystem::updateCamera() {
        camera.setViewYXZ(viewerObject.translation, viewerObject.rotation);
        camera.setReversePerspectiveProjection(glm::radians(80.0f), outputAspect.load(std::memory_order_relaxed));

        auto& frameInfo = frameInfos[frameInfoIndex];
        frameInfo.mvpUbo.projection = camera.projection();
        frameInfo.mvpUbo.view = camera.view();
        frameInfo.mvpUbo.inverseView = camera.inverseView();

        std::memcpy(frameInfo.mvpBuffer->memory(), &frameInfo.mvpUbo, sizeof(frameInfo.mvpUbo));
        frameInfo.mvpBuffer->flush();
    }

    void MainSystem::updateObjects() {
        if (boundWindow == nullptr) {
            updateCamera();
            return;
        }

        const float yaw = viewerObject.rotation.y;

        const glm::vec3 forwardDir { glm::sin(yaw), 0.0f, glm::cos(yaw) };
        const glm::vec3 rightDir { forwardDir.z, 0.0f, -forwardDir.x };
        const glm::vec3 upDir { 0.0f, 1.0f, 0.0f };

        glm::vec3 moveDir { 0.0f };
        if (boundWindow->isButtonPressed(coffee::Keys::S)) moveDir -= forwardDir;
        if (boundWindow->isButtonPressed(coffee::Keys::W)) moveDir += forwardDir;
        if (boundWindow->isButtonPressed(coffee::Keys::A)) moveDir -= rightDir;
        if (boundWindow->isButtonPressed(coffee::Keys::D)) moveDir += rightDir;
        if (boundWindow->isButtonPressed(coffee::Keys::LeftShift)) moveDir -= upDir;
        if (boundWindow->isButtonPressed(coffee::Keys::Space)) moveDir += upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            viewerObject.translation += moveSpeed * loopHandler.deltaTime() * glm::normalize(moveDir);
        }

        updateCamera();
    }

    void MainSystem::updateLightPoints() {
        auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, loopHandler.deltaTime(), { 0.0f, -1.0f, 0.0f });
        auto& frameInfo = frameInfos[frameInfoIndex];

        for (size_t i = 0; i < pointLights.size(); i++) {
            pointLights[i].transform.translation = glm::vec3(rotateLight * glm::vec4 { pointLights[i].transform.translation, 1.0f });

            frameInfo.lightUbo.lightPoints[i].position = glm::vec4(pointLights[i].transform.translation, pointLights[i].transform.scale.r);
            frameInfo.lightUbo.lightPoints[i].color = glm::vec4(pointLights[i].color, pointLights[i].intensity);
        }

        frameInfo.lightUbo.size = static_cast<uint32_t>(pointLights.size());

        std::memcpy(frameInfo.lightBuffer->memory(), &frameInfo.lightUbo, sizeof(frameInfo.lightUbo));
        frameInfo.lightBuffer->flush();
    }

    void MainSystem::createSamplers() {
        textureSampler = coffee::Sampler::create(device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .anisotropyEnable = true,
            .maxAnisotropy = 256U, // Set to huge value that will be rounded down to device maximum
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = VK_LOD_CLAMP_NONE,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
        });

        outputSampler = coffee::Sampler::create(device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .minLod = 0.0f,
            .maxLod = 1.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
        });
    }

    void MainSystem::createBuffers() {
        for (auto& frameInfo : frameInfos) {
            frameInfo.mvpBuffer = coffee::Buffer::create(device, {
                .instanceSize = sizeof(MVPUniformBuffer),
                .instanceCount = 1U,
                .usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            });

            frameInfo.lightBuffer = coffee::Buffer::create(device, {
                .instanceSize = sizeof(LightUniformBuffer),
                .instanceCount = 1U,
                .usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            });
        }
    }

    void MainSystem::loadModels() {
        sponzaModel = std::make_unique<Model>(device, assetManager->getModel(filesystem, "sponza_scene.cfa"), textureSampler);

        viewerObject.translation.z = -1.5f;
        viewerObject.translation.y = 1.0f;
    }

    void MainSystem::createRenderPasses() {
        earlyDepthPass = coffee::RenderPass::create(device, {
            .depthStencilAttachment = coffee::AttachmentConfiguration {
                .format = device->optimalDepthFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            }
        });

        renderPass = coffee::RenderPass::create(device, {
            .colorAttachments = { coffee::AttachmentConfiguration {
                .format = device->surfaceFormat().format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            }},
            .depthStencilAttachment = coffee::AttachmentConfiguration {
                .format = device->optimalDepthFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
            }
        });
    }

    void MainSystem::createPipelines() {
        earlyDepthPipeline = coffee::Pipeline::create(device, earlyDepthPass, {
            .shaders = coffee::utils::moveList<coffee::ShaderPtr, std::vector<coffee::ShaderPtr>>({
                assetManager->getShader(filesystem, "shaders/early_depth.vert.spv", VK_SHADER_STAGE_VERTEX_BIT)
            }),
            .layouts = {
                gameLayout
            },
            .inputBindings = { coffee::InputBinding {
                .binding = 0U,
                .stride = sizeof(coffee::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = coffee::Vertex::getElementDescriptions()
            }},
            .constantRanges = { coffee::PushConstantRange { 
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

        mainPipeline = coffee::Pipeline::create(device, renderPass, {
            .shaders = coffee::utils::moveList<coffee::ShaderPtr, std::vector<coffee::ShaderPtr>>({
                assetManager->getShader(filesystem, "shaders/base.vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
                assetManager->getShader(filesystem, "shaders/base.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)
            }),
            .layouts = {
                gameLayout,
                sponzaModel->layout
            },
            .inputBindings = { coffee::InputBinding {
                .binding = 0U,
                .stride = sizeof(coffee::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = coffee::Vertex::getElementDescriptions()
            }},
            .constantRanges = { coffee::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .size = static_cast<uint32_t>(sizeof(MainPushConstants))
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_BACK_BIT
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_EQUAL
            }
        });

        lightPointsPipeline = coffee::Pipeline::create(device, renderPass, {
            .shaders = coffee::utils::moveList<coffee::ShaderPtr, std::vector<coffee::ShaderPtr>>({
                assetManager->getShader(filesystem, "shaders/point_light.vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
                assetManager->getShader(filesystem, "shaders/point_light.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)
            }),
            .layouts = {
                gameLayout
            },
            .constantRanges = { coffee::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .size = static_cast<uint32_t>(sizeof(LightPushConstants))
            }},
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_BACK_BIT
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL
            }
        });
    }

    void MainSystem::createImages() {
        colorImage = coffee::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->surfaceFormat().format,
            .extent = { 1920U, 1080U },
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .priority = 1.0f
        });
        colorImageView = coffee::ImageView::create(colorImage, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = device->surfaceFormat().format
        });

        depthImage = coffee::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->optimalDepthFormat(),
            .extent = { 1920U, 1080U },
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            .priority = 1.0f
        });
        depthImageView = coffee::ImageView::create(depthImage, {
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

    void MainSystem::createFramebuffer() {
        earlyDepthFramebuffer = coffee::Framebuffer::create(device, earlyDepthPass, {
            .extent = { depthImage->extent.width, depthImage->extent.height },
            .depthStencilView = depthImageView
        });

        framebuffer = coffee::Framebuffer::create(device, renderPass, {
            .extent = { colorImage->extent.width, colorImage->extent.height },
            .colorViews = { colorImageView },
            .depthStencilView = depthImageView
        });
    }

    void MainSystem::updateDescriptors() {
        outputSet->updateDescriptor(
            coffee::DescriptorWriter(outputLayout).addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, colorImageView, textureSampler));
    }

    void MainSystem::createDescriptors() {
        gameLayout = coffee::DescriptorLayout::create(device, {
            { 0, {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .shaderStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT } },
            { 1, {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .shaderStages = VK_SHADER_STAGE_FRAGMENT_BIT } }
        });

        outputLayout = coffee::DescriptorLayout::create(device, {
            { 0, {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .shaderStages = VK_SHADER_STAGE_FRAGMENT_BIT } }
        });

        for (auto& frameInfo : frameInfos) {
            coffee::DescriptorWriter writer = coffee::DescriptorWriter(gameLayout)
                .addBuffer(0, frameInfo.mvpBuffer)
                .addBuffer(1, frameInfo.lightBuffer);

            frameInfo.descriptorSet = coffee::DescriptorSet::create(device, writer);
        }

        outputSet = coffee::DescriptorSet::create(device,
            coffee::DescriptorWriter(outputLayout).addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, colorImageView, textureSampler));
    }

    void MainSystem::mousePositionCallback(const coffee::Window& window, const coffee::MouseMoveEvent& e) noexcept {
        if (window.cursorState() != coffee::CursorState::Disabled) {
            return;
        }

        coffee::Float2D mousePosition = window.mousePosition();
        viewerObject.rotation.x =
            glm::clamp(viewerObject.rotation.x + lookSpeed * (e.y - mousePosition.y), -1.5f, 1.5f);
        viewerObject.rotation.y =
            glm::mod(viewerObject.rotation.y + lookSpeed * (e.x - mousePosition.x), glm::two_pi<float>());
    }

    void MainSystem::keyCallback(const coffee::Window& window, const coffee::KeyEvent& e) noexcept {
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

}