#include "main_behaviour.hpp"

#include <coffee/objects/vertex.hpp>
#include <coffee/engine.hpp>

#include <imgui.h>

#include "structs.hpp"

namespace game {

    MainSystem::MainSystem(coffee::Engine& engine) : engine { engine } {
        createSamplers();
        createBuffers();
        loadModels();
        createImages();
        createDescriptors();
        createRenderPasses();
        createFramebuffer();
        createPipelines();

        engine.addWindowResizeCallback("main", [this](const coffee::ResizeEvent&) {
            createImages();
            createFramebuffer();
            updateDescriptors();
        });

        const size_t presentImagesSize = engine.getPresentImages().size();
        StaticObjects::mvpUniformBuffers.resize(presentImagesSize);
        StaticObjects::lightUniformBuffers.resize(presentImagesSize);

        for (auto& currentLightBuffer : StaticObjects::lightUniformBuffers) {
            currentLightBuffer.lightPoint.position = { -1.0f, 1.0f, -1.0f, 1.0f }; // w component is ignored
            currentLightBuffer.lightPoint.color = { 1.0f, 1.0f, 1.0f, 4.0f }; // rgb and w component as intensity
        }

        engine.addMouseClickCallback("main", [&](const coffee::MouseClickEvent& e) {
            if (e.getButton() == coffee::MouseButton::Right) {
                engine.disableCursor();
                engine.disableTextMode();
            }
        });

        engine.addMousePositionCallback("main", [&](const coffee::MouseMoveEvent& e) {
            if (engine.getCursorState() != coffee::CursorState::Disabled || engine.isTextMode()) {
                return;
            }

            const float lookDistance = lookSpeed * engine.getDeltaTime();
            viewerObject.transform.rotation.x =
                glm::clamp(viewerObject.transform.rotation.x + lookDistance * (e.getY() - engine.getMouseY()), -1.5f, 1.5f);
            viewerObject.transform.rotation.y =
                glm::mod(viewerObject.transform.rotation.y + lookDistance * (e.getX() - engine.getMouseX()), glm::two_pi<float>());
        });

        engine.addKeyCallback("main", [&](const coffee::KeyEvent& e) {
            switch (e.getKey()) {
                case coffee::Keys::Escape:
                    engine.showCursor();
                    break;
                case coffee::Keys::Enter:
                    engine.showCursor();
                    engine.enableTextMode();
                    break;
                case coffee::Keys::W:
                case coffee::Keys::A:
                case coffee::Keys::S:
                case coffee::Keys::D:
                    if (!engine.isTextMode()) {
                        engine.disableCursor();
                    }
                    break;
                default:
                    break;
            }
        });
    }

    MainSystem::~MainSystem() {
        engine.waitDeviceIdle();

        engine.removeKeyCallback("main");
        engine.removeMousePositionCallback("main");
        engine.removeMouseClickCallback("main");
    }

    void MainSystem::performDepthPass() {

    }

    void MainSystem::updateObjects() {
        if (engine.isTextMode()) {
            return;
        }

        const float yaw = viewerObject.transform.rotation.y;

        const glm::vec3 forwardDir { sin(yaw), 0.0f, cos(yaw) };
        const glm::vec3 rightDir { forwardDir.z, 0.0f, -forwardDir.x };
        const glm::vec3 upDir { 0.0f, 1.0f, 0.0f };

        glm::vec3 moveDir { 0.0f };
        if (engine.isButtonPressed(coffee::Keys::S)) moveDir -= forwardDir;
        if (engine.isButtonPressed(coffee::Keys::W)) moveDir += forwardDir;
        if (engine.isButtonPressed(coffee::Keys::A)) moveDir -= rightDir;
        if (engine.isButtonPressed(coffee::Keys::D)) moveDir += rightDir;
        if (engine.isButtonPressed(coffee::Keys::LeftShift)) moveDir -= upDir;
        if (engine.isButtonPressed(coffee::Keys::Space)) moveDir += upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            viewerObject.transform.translation += moveSpeed * engine.getDeltaTime() * glm::normalize(moveDir);
        }

        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        camera.setPerspectiveProjection(
            glm::radians(80.0f), 
            static_cast<float>(engine.getFramebufferWidth()) / engine.getFramebufferHeight(), 
            0.1f,
            1000.0f);

        auto& mvp = StaticObjects::mvpUniformBuffers[engine.getCurrentImageIndex()];
        mvp.projection = camera.getProjection();
        mvp.view = camera.getView();
        mvp.inverseView = camera.getInverseView();
    }

    void MainSystem::updateLightPoints() {
        //auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, engine.getDeltaTime(), { 0.0f, -1.0f, 0.0f });
        //LightUniformBuffer& currentLightBuffer = StaticObjects::lightUniformBuffers[engine.getCurrentImageIndex()];

        //for (size_t i = 0; i < lights.size(); i++) {
        //    lights[i].transform.translation = glm::vec3(rotateLight * glm::vec4 { lights[i].transform.translation, 1.0f });

        //    currentLightBuffer.lightPoints[i].position = glm::vec4(lights[i].transform.translation, 1.0f);
        //    currentLightBuffer.lightPoints[i].color = glm::vec4(lights[i].color, lights[i].intensity);
        //}

        //currentLightBuffer.length = static_cast<uint32_t>(lights.size());
    }

    void MainSystem::renderObjects(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->bindPipeline(mainPipeline);
        commandBuffer->setViewport({ engine.getFramebufferWidth(), engine.getFramebufferHeight() });
        commandBuffer->setScissor({ engine.getFramebufferWidth(), engine.getFramebufferHeight() });

        mvpBuffer->write(StaticObjects::mvpUniformBuffers[engine.getCurrentImageIndex()]);
        mvpBuffer->flush();
        lightBuffer->write(StaticObjects::lightUniformBuffers[engine.getCurrentImageIndex()]);
        lightBuffer->flush();

        mainConstants.transform = sponzaModel->transform.mat4();
        mainConstants.normal = sponzaModel->transform.normal();
        commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, mainConstants);

        const auto& meshes = sponzaModel->model->meshes;
        for (size_t i = 0; i < meshes.size(); i++) {
            commandBuffer->bindDescriptorSets({ descriptorSet, sponzaModel->descriptors[i] });
            meshes[i]->draw(commandBuffer);
        }
    }

    void MainSystem::renderLightPoints(const coffee::CommandBuffer& commandBuffer) {
        LightUniformBuffer& currentLightBuffer = StaticObjects::lightUniformBuffers[engine.getCurrentImageIndex()];

        commandBuffer->bindPipeline(lightPointsPipeline);
        commandBuffer->bindDescriptorSet(descriptorSet);
        commandBuffer->setViewport({ engine.getFramebufferWidth(), engine.getFramebufferHeight() });
        commandBuffer->setScissor({ engine.getFramebufferWidth(), engine.getFramebufferHeight() });

        mvpBuffer->write(StaticObjects::mvpUniformBuffers[engine.getCurrentImageIndex()]);
        mvpBuffer->flush();

        lightPointsConstants.position = currentLightBuffer.lightPoint.position;
        lightPointsConstants.color = currentLightBuffer.lightPoint.color;
        lightPointsConstants.radius = 0.2f;

        commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, lightPointsConstants);
        commandBuffer->draw(6);
    }

    void MainSystem::beginRenderPass(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->beginRenderPass(renderPass, framebuffer, { engine.getFramebufferWidth(), engine.getFramebufferHeight() });
    }

    void MainSystem::endRenderPass(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->endRenderPass();
    }

    void MainSystem::createSamplers() {
        coffee::SamplerConfiguration textureSamplerConfiguration {};
        textureSamplerConfiguration.magFilter = coffee::SamplerFilter::Linear;
        textureSamplerConfiguration.minFilter = coffee::SamplerFilter::Linear;
        textureSamplerConfiguration.mipmapMode = coffee::SamplerFilter::Linear;
        textureSamplerConfiguration.anisotropyEnable = true;
        textureSamplerConfiguration.maxAnisotropy = 256U; // Set to huge value that will be rounded down to device maximum
        textureSamplerConfiguration.compareOp = coffee::CompareOp::Always;
        textureSamplerConfiguration.borderColor = coffee::BorderColor::OpaqueBlack;
        textureSampler = engine.createSampler(textureSamplerConfiguration);

        coffee::SamplerConfiguration samplerConfiguration {};
        samplerConfiguration.magFilter = coffee::SamplerFilter::Linear;
        samplerConfiguration.minFilter = coffee::SamplerFilter::Linear;
        samplerConfiguration.mipmapMode = coffee::SamplerFilter::Linear;
        samplerConfiguration.addressModeU = coffee::AddressMode::ClampToEdge;
        samplerConfiguration.addressModeV = coffee::AddressMode::ClampToEdge;
        samplerConfiguration.addressModeW = coffee::AddressMode::ClampToEdge;
        samplerConfiguration.borderColor = coffee::BorderColor::OpaqueWhite;
        samplerConfiguration.minLod = 0.0f;
        samplerConfiguration.maxLod = 1.0f;
        outputSampler = engine.createSampler(samplerConfiguration);
    }

    void MainSystem::createBuffers() {
        coffee::BufferConfiguration configuration{};
        configuration.usage = coffee::BufferUsage::Uniform;
        configuration.properties = coffee::MemoryProperty::HostVisible;
        configuration.instanceCount = 1U;

        configuration.instanceSize = sizeof(MVPUniformBuffer);
        mvpBuffer = engine.createBuffer(configuration);

        configuration.instanceSize = sizeof(LightUniformBuffer);
        lightBuffer = engine.createBuffer(configuration);
    }

    void MainSystem::loadModels() {
        sponzaModel = std::make_unique<Model>(engine, "models/sponza_scene.glb", TransformComponent {}, textureSampler);

        viewerObject.transform.translation.z = -2.5f;
    }

    void MainSystem::createRenderPasses() {
        coffee::RenderPassConfiguration renderPassConfiguration {};
        coffee::AttachmentConfiguration depthStencilAttachment {};

        auto& colorAttachment = renderPassConfiguration.colorAttachments.emplace_back();

        colorAttachment.sampleCount = 1U;
        colorAttachment.initialState = coffee::ResourceState::Undefined;
        colorAttachment.finalState = coffee::ResourceState::ShaderResource;
        colorAttachment.format = engine.getColorFormat();
        colorAttachment.loadOp = coffee::AttachmentLoadOp::Clear;
        colorAttachment.storeOp = coffee::AttachmentStoreOp::Store;
        colorAttachment.stencilLoadOp = coffee::AttachmentLoadOp::DontCare;
        colorAttachment.stencilStoreOp = coffee::AttachmentStoreOp::DontCare;
        colorAttachment.clearValue = std::array<float, 4> { 0.01f, 0.01f, 0.01f, 1.0f };

        depthStencilAttachment.initialState = coffee::ResourceState::Undefined;
        depthStencilAttachment.finalState = coffee::ResourceState::DepthWrite;
        depthStencilAttachment.format = engine.getDepthFormat();
        depthStencilAttachment.loadOp = coffee::AttachmentLoadOp::Clear;
        depthStencilAttachment.storeOp = coffee::AttachmentStoreOp::DontCare;
        depthStencilAttachment.stencilLoadOp = coffee::AttachmentLoadOp::DontCare;
        depthStencilAttachment.stencilStoreOp = coffee::AttachmentStoreOp::DontCare;
        depthStencilAttachment.depthStencilClearValue = coffee::ClearDepthStencilValue { 1.0f, 0U };

        renderPassConfiguration.depthStencilAttachment = depthStencilAttachment;

        renderPass = engine.createRenderPass(renderPassConfiguration);
    }

    void MainSystem::createPipelines() {
        coffee::PushConstants mainPushConstants {};
        coffee::ShaderProgram mainProgram {};
        coffee::PushConstants lightPointsConstants {};
        coffee::ShaderProgram lightPointsProgram {};

        mainPushConstants
            .addRange(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment)
            .addObject<MainPushConstants>();

        lightPointsConstants
            .addRange(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment)
            .addObject<LightPushConstants>();

        mainProgram
            .addVertexShader(engine.createShader("shaders/base.vert.spv", coffee::ShaderStage::Vertex))
            .addFragmentShader(engine.createShader("shaders/base.frag.spv", coffee::ShaderStage::Fragment));

        lightPointsProgram
            .addVertexShader(engine.createShader("shaders/point_light.vert.spv", coffee::ShaderStage::Vertex))
            .addFragmentShader(engine.createShader("shaders/point_light.frag.spv", coffee::ShaderStage::Fragment));

        mainPipeline = engine.createPipeline(renderPass, mainPushConstants, { layout, sponzaModel->layout }, mainProgram, []() {
            coffee::PipelineConfiguration configuration {};

            configuration.inputBindings.push_back(
                { 0U, sizeof(coffee::Vertex), coffee::InputRate::PerVertex, coffee::Vertex::getElementDescriptions() });

            return configuration;
        }());

        lightPointsPipeline = engine.createPipeline(renderPass, lightPointsConstants, { layout }, lightPointsProgram, {});
    }

    void MainSystem::createImages() {
        coffee::ImageConfiguration colorImageConfiguration {};
        colorImageConfiguration.type = coffee::ImageType::TwoDimensional;
        colorImageConfiguration.format = engine.getColorFormat();
        colorImageConfiguration.width = engine.getFramebufferWidth();
        colorImageConfiguration.height = engine.getFramebufferHeight();
        colorImageConfiguration.usage = coffee::ImageUsage::ColorAttachment | coffee::ImageUsage::Sampled;
        colorImageConfiguration.initialState = coffee::ResourceState::Undefined;
        colorImageConfiguration.viewType = coffee::ImageViewType::TwoDimensional;
        colorImageConfiguration.aspects = coffee::ImageAspect::Color;
        colorImage = engine.createImage(colorImageConfiguration);

        coffee::ImageConfiguration depthImageConfiguration {};
        depthImageConfiguration.type = coffee::ImageType::TwoDimensional;
        depthImageConfiguration.format = engine.getDepthFormat();
        depthImageConfiguration.width = engine.getFramebufferWidth();
        depthImageConfiguration.height = engine.getFramebufferHeight();
        depthImageConfiguration.usage = coffee::ImageUsage::DepthStencilAttachment;
        depthImageConfiguration.initialState = coffee::ResourceState::Undefined;
        depthImageConfiguration.viewType = coffee::ImageViewType::TwoDimensional;
        depthImageConfiguration.aspects = coffee::ImageAspect::Depth;
        depthImage = engine.createImage(depthImageConfiguration);
    }

    void MainSystem::createFramebuffer() {
        coffee::FramebufferConfiguration framebufferConfiguration {};
        framebufferConfiguration.width = engine.getFramebufferWidth();
        framebufferConfiguration.height = engine.getFramebufferHeight();
        framebufferConfiguration.colorViews.push_back(colorImage);
        framebufferConfiguration.depthStencilView = depthImage;
        framebuffer = engine.createFramebuffer(renderPass, framebufferConfiguration);
    }

    void MainSystem::updateDescriptors() {
        outputSet->updateDescriptor(
            coffee::DescriptorWriter(layout).addImage(0, coffee::ResourceState::ShaderResource, colorImage, textureSampler));
    }

    void MainSystem::createDescriptors() {
        std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {};

        bindings[0] = { coffee::DescriptorType::UniformBuffer, coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment };
        bindings[1] = { coffee::DescriptorType::UniformBuffer, coffee::ShaderStage::Fragment };

        layout = engine.createDescriptorLayout(bindings);
        descriptorSet = engine.createDescriptorSet(coffee::DescriptorWriter(layout).addBuffer(0, mvpBuffer).addBuffer(1, lightBuffer));

        std::map<uint32_t, coffee::DescriptorBindingInfo> outputBindings {};

        outputBindings[0] = { coffee::DescriptorType::ImageAndSampler, coffee::ShaderStage::Fragment };

        outputLayout = engine.createDescriptorLayout(outputBindings);
        outputSet = engine.createDescriptorSet(
            coffee::DescriptorWriter(outputLayout).addImage(0, coffee::ResourceState::ShaderResource, colorImage, textureSampler));
    }

}