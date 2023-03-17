#include "main_behaviour.hpp"

#include <coffee/objects/vertex.hpp>
#include <coffee/engine.hpp>

#include <imgui.h>

#include "structs.hpp"

namespace game {

    std::vector<glm::vec4> lightColors {
        {1.0f, 0.1f, 0.1f, 3.0f},
        {0.1f, 0.1f, 1.0f, 3.0f},
        {0.1f, 1.0f, 0.1f, 3.0f},
        {1.0f, 1.0f, 0.1f, 3.0f},
        {0.1f, 1.0f, 1.0f, 3.0f},
        {1.0f, 1.0f, 1.0f, 3.0f}
    };

    MainSystem::MainSystem(coffee::Window& window) : window { window } {
        frameInfos.resize(window->getPresentImages().size());

        createSamplers();
        createBuffers();
        loadModels();
        createImages();
        createDescriptors();
        createRenderPasses();
        createFramebuffer();
        createPipelines();

        window->addWindowResizeCallback("main", [this](const coffee::AbstractWindow* const, const coffee::ResizeEvent&) {
            createImages();
            createFramebuffer();
            updateDescriptors();
        });

        for (size_t i = 0; i < lightColors.size(); i++) {
            LightObject& object = lights.emplace_back(LightObject::createLightObject(lightColors[i].a));
            object.color = lightColors[i];
            auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, (i * glm::two_pi<float>()) / lightColors.size(), { 0.0f, -1.0f, 0.0f });
            object.transform.translation = glm::vec3(rotateLight * glm::vec4 { -0.5f, 1.0f, -0.5f, 1.0f });
        }

        window->addMouseClickCallback("main", [](const coffee::AbstractWindow* const window, const coffee::MouseClickEvent& e) {
            if (e.getButton() == coffee::MouseButton::Right) {
                window->disableCursor();
                window->disableTextMode();
            }
        });

        window->addMousePositionCallback("main",
        [&look = lookSpeed, &viewer = viewerObject](
            const coffee::AbstractWindow* const window, 
            const coffee::MouseMoveEvent& e
        ) {
            if (window->getCursorState() != coffee::CursorState::Disabled || window->isTextMode()) {
                return;
            }

            coffee::Float2D mousePosition = window->getMousePosition();
            viewer.transform.rotation.x =
                glm::clamp(viewer.transform.rotation.x + look * (e.getY() - mousePosition.y), -1.5f, 1.5f);
            viewer.transform.rotation.y =
                glm::mod(viewer.transform.rotation.y + look * (e.getX() - mousePosition.x), glm::two_pi<float>());
        });

        window->addKeyCallback("main", [](const coffee::AbstractWindow* const window, const coffee::KeyEvent& e) {
            switch (e.getKey()) {
                case coffee::Keys::Escape:
                    window->showCursor();
                    break;
                case coffee::Keys::Enter:
                    window->showCursor();
                    window->enableTextMode();
                    break;
                case coffee::Keys::W:
                case coffee::Keys::A:
                case coffee::Keys::S:
                case coffee::Keys::D:
                    if (!window->isTextMode()) {
                        window->disableCursor();
                    }
                    break;
                default:
                    break;
            }
        });
    }

    MainSystem::~MainSystem() {
        coffee::Engine::waitDeviceIdle();

        window->removeKeyCallback("main");
        window->removeMousePositionCallback("main");
        window->removeMouseClickCallback("main");
    }

    void MainSystem::performDepthPass(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->beginRenderPass(earlyDepthPass, earlyDepthFramebuffer, window->getFramebufferSize());

        commandBuffer->setViewport(window->getFramebufferSize());
        commandBuffer->setScissor(window->getFramebufferSize());
        commandBuffer->bindPipeline(earlyDepthPipeline);
        commandBuffer->bindDescriptorSet(frameInfos[window->getCurrentImageIndex()].descriptorSet);
        commandBuffer->pushConstants(coffee::ShaderStage::Vertex, sponzaModel->transform.mat4());

        const auto& meshes = sponzaModel->model->meshes;
        for (size_t i = 0; i < meshes.size(); i++) {
            meshes[i]->draw(commandBuffer);
        }

        commandBuffer->endRenderPass();
    }

    void MainSystem::updateObjects() {
        if (window->isTextMode()) {
            return;
        }

        const float yaw = viewerObject.transform.rotation.y;

        const glm::vec3 forwardDir { sin(yaw), 0.0f, cos(yaw) };
        const glm::vec3 rightDir { forwardDir.z, 0.0f, -forwardDir.x };
        const glm::vec3 upDir { 0.0f, 1.0f, 0.0f };

        glm::vec3 moveDir { 0.0f };
        if (window->isButtonPressed(coffee::Keys::S)) moveDir -= forwardDir;
        if (window->isButtonPressed(coffee::Keys::W)) moveDir += forwardDir;
        if (window->isButtonPressed(coffee::Keys::A)) moveDir -= rightDir;
        if (window->isButtonPressed(coffee::Keys::D)) moveDir += rightDir;
        if (window->isButtonPressed(coffee::Keys::LeftShift)) moveDir -= upDir;
        if (window->isButtonPressed(coffee::Keys::Space)) moveDir += upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            viewerObject.transform.translation += moveSpeed * coffee::Engine::getDeltaTime() * glm::normalize(moveDir);
        }

        coffee::Extent2D framebufferSize = window->getFramebufferSize();
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        camera.setPerspectiveProjection(
            glm::radians(80.0f), 
            static_cast<float>(framebufferSize.width) / framebufferSize.height,
            0.1f,
            1000.0f);

        auto& frameInfo = frameInfos[window->getCurrentImageIndex()];
        frameInfo.mvpUbo.projection = camera.getProjection();
        frameInfo.mvpUbo.view = camera.getView();
        frameInfo.mvpUbo.inverseView = camera.getInverseView();

        frameInfo.mvpBuffer->write(frameInfo.mvpUbo);
        frameInfo.mvpBuffer->flush();
    }

    void MainSystem::updateLightPoints() {
        auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, coffee::Engine::getDeltaTime(), { 0.0f, -1.0f, 0.0f });
        auto& frameBuffer = frameInfos[window->getCurrentImageIndex()];

        for (size_t i = 0; i < lights.size(); i++) {
            lights[i].transform.translation = glm::vec3(rotateLight * glm::vec4 { lights[i].transform.translation, 1.0f });

            frameBuffer.lightUbo.lightPoints[i].position = glm::vec4(lights[i].transform.translation, lights[i].transform.scale.r);
            frameBuffer.lightUbo.lightPoints[i].color = glm::vec4(lights[i].color, lights[i].intensity);
        }

        frameBuffer.lightUbo.size = static_cast<uint32_t>(lights.size());

        frameBuffer.lightBuffer->write(frameBuffer.lightUbo);
        frameBuffer.lightBuffer->flush();
    }

    void MainSystem::renderObjects(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->bindPipeline(mainPipeline);
        commandBuffer->setViewport(window->getFramebufferSize());
        commandBuffer->setScissor(window->getFramebufferSize());

        mainConstants.transform = sponzaModel->transform.mat4();
        mainConstants.normal = sponzaModel->transform.normal();
        commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, mainConstants);

        const auto& meshes = sponzaModel->model->meshes;
        for (size_t i = 0; i < meshes.size(); i++) {
            commandBuffer->bindDescriptorSets({ frameInfos[window->getCurrentImageIndex()].descriptorSet, sponzaModel->descriptors[i]});
            meshes[i]->draw(commandBuffer);
        }
    }

    void MainSystem::renderLightPoints(const coffee::CommandBuffer& commandBuffer) {
        LightUniformBuffer& currentLightBuffer = frameInfos[window->getCurrentImageIndex()].lightUbo;

        commandBuffer->bindPipeline(lightPointsPipeline);
        commandBuffer->bindDescriptorSet(frameInfos[window->getCurrentImageIndex()].descriptorSet);
        commandBuffer->setViewport(window->getFramebufferSize());
        commandBuffer->setScissor(window->getFramebufferSize());

        for (uint32_t i = 0; i < currentLightBuffer.size; i++) {
            lightPointsConstants.position = currentLightBuffer.lightPoints[i].position;
            lightPointsConstants.color = currentLightBuffer.lightPoints[i].color;

            commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, lightPointsConstants);
            commandBuffer->draw(6);
        }
    }

    void MainSystem::beginRenderPass(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->beginRenderPass(renderPass, framebuffer, window->getFramebufferSize());
    }

    void MainSystem::endRenderPass(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->endRenderPass();
    }

    void MainSystem::createSamplers() {
        textureSampler = coffee::Factory::createSampler({
            .magFilter = coffee::SamplerFilter::Linear,
            .minFilter = coffee::SamplerFilter::Linear,
            .mipmapMode = coffee::SamplerFilter::Linear,
            .anisotropyEnable = true,
            .maxAnisotropy = 256U, // Set to huge value that will be rounded down to device maximum
            .compareOp = coffee::CompareOp::Always,
            .borderColor = coffee::BorderColor::OpaqueBlack
        });

        outputSampler = coffee::Factory::createSampler({
            .magFilter = coffee::SamplerFilter::Linear,
            .minFilter = coffee::SamplerFilter::Linear,
            .mipmapMode = coffee::SamplerFilter::Linear,
            .addressModeU = coffee::AddressMode::ClampToEdge,
            .addressModeV = coffee::AddressMode::ClampToEdge,
            .addressModeW = coffee::AddressMode::ClampToEdge,
            .borderColor = coffee::BorderColor::OpaqueWhite,
            .minLod = 0.0f,
            .maxLod = 1.0f
        });
    }

    void MainSystem::createBuffers() {
        for (auto& frameInfo : frameInfos) {
            frameInfo.mvpBuffer = coffee::Factory::createBuffer({
                .usage = coffee::BufferUsage::Uniform,
                .properties = coffee::MemoryProperty::HostVisible,
                .instanceCount = 1U,
                .instanceSize = sizeof(MVPUniformBuffer)
            });

            frameInfo.lightBuffer = coffee::Factory::createBuffer({
                .usage = coffee::BufferUsage::Uniform,
                .properties = coffee::MemoryProperty::HostVisible,
                .instanceCount = 1U,
                .instanceSize = sizeof(LightUniformBuffer)
            });
        }
    }

    void MainSystem::loadModels() {
        sponzaModel = std::make_unique<Model>(
            "models/sponza_scene.cfa", "models/sponza_scene_materials.cfz", TransformComponent {}, textureSampler);

        viewerObject.transform.translation.z = -1.5f;
        viewerObject.transform.translation.y = 1.0f;
    }

    void MainSystem::createRenderPasses() {
        earlyDepthPass = coffee::Factory::createRenderPass({
            .depthStencilAttachment = coffee::AttachmentConfiguration {
                .initialState = coffee::ResourceState::Undefined,
                .finalState = coffee::ResourceState::DepthWrite,
                .format = window->getDepthFormat(),
                .loadOp = coffee::AttachmentLoadOp::Clear,
                .storeOp = coffee::AttachmentStoreOp::Store,
                .stencilLoadOp = coffee::AttachmentLoadOp::DontCare,
                .stencilStoreOp = coffee::AttachmentStoreOp::DontCare,
                .depthStencilClearValue = coffee::ClearDepthStencilValue { 1.0f, 0U }
            }
        });

        renderPass = coffee::Factory::createRenderPass({
            .colorAttachments = {{
                .sampleCount = 1U,
                .initialState = coffee::ResourceState::Undefined,
                .finalState = coffee::ResourceState::ShaderResource,
                .format = window->getColorFormat(),
                .loadOp = coffee::AttachmentLoadOp::Clear,
                .storeOp = coffee::AttachmentStoreOp::Store,
                .stencilLoadOp = coffee::AttachmentLoadOp::DontCare,
                .stencilStoreOp = coffee::AttachmentStoreOp::DontCare,
                .clearValue = std::array<float, 4> { 0.01f, 0.01f, 0.01f, 1.0f }
            }},
            .depthStencilAttachment = coffee::AttachmentConfiguration {
                .initialState = coffee::ResourceState::DepthWrite,
                .finalState = coffee::ResourceState::DepthWrite,
                .format = window->getDepthFormat(),
                .loadOp = coffee::AttachmentLoadOp::Load,
                .storeOp = coffee::AttachmentStoreOp::Store,
                .stencilLoadOp = coffee::AttachmentLoadOp::DontCare,
                .stencilStoreOp = coffee::AttachmentStoreOp::DontCare
            }
        });
    }

    void MainSystem::createPipelines() {
        std::vector<coffee::Shader> earlyDepthShaders {};
        earlyDepthShaders.push_back(coffee::Factory::createShader("shaders/early_depth.vert.spv", coffee::ShaderStage::Vertex));

        earlyDepthPipeline = coffee::Factory::createPipeline(earlyDepthPass, { layout }, earlyDepthShaders, {
            .inputBindings = {{ 0U, sizeof(coffee::Vertex), coffee::InputRate::PerVertex, coffee::Vertex::getElementDescriptions() }},
            .ranges = { coffee::PushConstantsRange { 
                .shaderStages = coffee::ShaderStage::Vertex, 
                .size = static_cast<uint32_t>(sizeof(glm::mat4))
            }}
        });

        std::vector<coffee::Shader> mainShaders {};
        mainShaders.push_back(coffee::Factory::createShader("shaders/base.vert.spv", coffee::ShaderStage::Vertex));
        mainShaders.push_back(coffee::Factory::createShader("shaders/base.frag.spv", coffee::ShaderStage::Fragment));

        mainPipeline = coffee::Factory::createPipeline(renderPass, { layout, sponzaModel->layout }, mainShaders, {
            .inputBindings = {{ 0U, sizeof(coffee::Vertex), coffee::InputRate::PerVertex, coffee::Vertex::getElementDescriptions() }},
            .ranges = { coffee::PushConstantsRange {
                .shaderStages = coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment,
                .size = static_cast<uint32_t>(sizeof(MainPushConstants))
            }}
        });

        std::vector<coffee::Shader> lightPointsShaders {};
        lightPointsShaders.push_back(coffee::Factory::createShader("shaders/point_light.vert.spv", coffee::ShaderStage::Vertex));
        lightPointsShaders.push_back(coffee::Factory::createShader("shaders/point_light.frag.spv", coffee::ShaderStage::Fragment));

        lightPointsPipeline = coffee::Factory::createPipeline(renderPass, { layout }, lightPointsShaders, {
            .ranges = { coffee::PushConstantsRange { 
                .shaderStages = coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment,
                .size = static_cast<uint32_t>(sizeof(LightPushConstants))
            }}
        });
    }

    void MainSystem::createImages() {
        colorImage = coffee::Factory::createImage({
            .type = coffee::ImageType::TwoDimensional,
            .format = window->getColorFormat(),
            .extent = window->getFramebufferSize(),
            .usage = coffee::ImageUsage::ColorAttachment | coffee::ImageUsage::Sampled,
            .initialState = coffee::ResourceState::Undefined,
            .viewType = coffee::ImageViewType::TwoDimensional,
            .aspects = coffee::ImageAspect::Color
        });

        depthImage = coffee::Factory::createImage({
            .type = coffee::ImageType::TwoDimensional,
            .format = window->getDepthFormat(),
            .extent = window->getFramebufferSize(),
            .usage = coffee::ImageUsage::DepthStencilAttachment,
            .initialState = coffee::ResourceState::Undefined,
            .viewType = coffee::ImageViewType::TwoDimensional,
            .aspects = coffee::ImageAspect::Depth
        });
    }

    void MainSystem::createFramebuffer() {
        earlyDepthFramebuffer = coffee::Factory::createFramebuffer(earlyDepthPass, {
            .extent = window->getFramebufferSize(),
            .depthStencilView = depthImage
        });

        framebuffer = coffee::Factory::createFramebuffer(renderPass, {
            .extent = window->getFramebufferSize(),
            .colorViews = { colorImage },
            .depthStencilView = depthImage
        });
    }

    void MainSystem::updateDescriptors() {
        outputSet->updateDescriptor(
            coffee::DescriptorWriter(outputLayout).addImage(0, coffee::ResourceState::ShaderResource, colorImage, textureSampler));
    }

    void MainSystem::createDescriptors() {
        std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {};

        bindings[0] = { coffee::DescriptorType::UniformBuffer, coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment };
        bindings[1] = { coffee::DescriptorType::UniformBuffer, coffee::ShaderStage::Fragment };

        layout = coffee::Factory::createDescriptorLayout(bindings);

        for (auto& frameInfo : frameInfos) {
            frameInfo.descriptorSet = 
                coffee::Factory::createDescriptorSet(coffee::DescriptorWriter(layout)
                    .addBuffer(0, frameInfo.mvpBuffer)
                    .addBuffer(1, frameInfo.lightBuffer));
        }

        std::map<uint32_t, coffee::DescriptorBindingInfo> outputBindings {};

        outputBindings[0] = { coffee::DescriptorType::ImageAndSampler, coffee::ShaderStage::Fragment };

        outputLayout = coffee::Factory::createDescriptorLayout(outputBindings);
        outputSet = coffee::Factory::createDescriptorSet(
            coffee::DescriptorWriter(outputLayout).addImage(0, coffee::ResourceState::ShaderResource, colorImage, textureSampler));
    }

}