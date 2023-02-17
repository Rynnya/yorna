#include "main_behaviour.hpp"

#include <coffee/engine.hpp>

#include <imgui.h>

#include "structs.hpp"

namespace game {

    MainSystem::MainSystem(
        coffee::Engine& engine,
        const coffee::RenderPass& renderPass
    ) : engine { engine } {
        createDescriptorLayout();
        createBuffers();
        createDescriptorSet();
        createPipeline(renderPass);

        const size_t presentImagesSize = engine.getPresentImages().size();
        StaticObjects::mvpUniformBuffers.resize(presentImagesSize);
        StaticObjects::lightUniformBuffers.resize(presentImagesSize);

        smoothVase = ModelObject::createModelObject(createModel("models/smooth_vase.obj"));
        smoothVase.transform.translation = { 0.5f, 0.5f, 0.0f };
        smoothVase.transform.scale = { 2.5f, 1.0f, 2.5f };

        flatVase = ModelObject::createModelObject(createModel("models/flat_vase.obj"));
        flatVase.transform.translation = { -0.5f, 0.5f, 0.0f };
        flatVase.transform.scale = { 2.5f, 1.0f, 2.5f };

        floor = ModelObject::createModelObject(createModel("models/quad.obj"));
        floor.transform.translation = { 0.0f, 0.5f, 0.0f };
        floor.transform.scale = { 3.0f, 1.0f, 3.0f };

        viewerObject.transform.translation.z = -2.5f;

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
                glm::clamp(viewerObject.transform.rotation.x + lookDistance * (engine.getMouseY() - e.getY()), -1.5f, 1.5f);
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

    void MainSystem::update() {
        if (engine.isTextMode()) {
            return;
        }

        const float yaw = viewerObject.transform.rotation.y;

        const glm::vec3 forwardDir { sin(yaw), 0.0f, cos(yaw) };
        const glm::vec3 rightDir { forwardDir.z, 0.0f, -forwardDir.x };
        const glm::vec3 upDir { 0.0f, -1.0f, 0.0f };

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
        camera.setPerspectiveProjection(glm::radians(50.0f), static_cast<float>(engine.getFramebufferWidth()) / engine.getFramebufferHeight());

        auto& mvp = StaticObjects::mvpUniformBuffers[engine.getCurrentImageIndex()];
        mvp.projection = camera.getProjection();
        mvp.view = camera.getView();
        mvp.inverseView = camera.getInverseView();
    }

    void MainSystem::render(const coffee::CommandBuffer& commandBuffer) {
        commandBuffer->bindDescriptorSets(pipeline, descriptorSet);
        commandBuffer->bindPipeline(pipeline);
        commandBuffer->setViewport(engine.getFramebufferWidth(), engine.getFramebufferHeight());
        commandBuffer->setScissor(engine.getFramebufferWidth(), engine.getFramebufferHeight());

        std::memcpy(mvpBuffer->map(), &StaticObjects::mvpUniformBuffers[engine.getCurrentImageIndex()], sizeof(MVPUniformBuffer));
        mvpBuffer->flush();
        std::memcpy(lightBuffer->map(), &StaticObjects::lightUniformBuffers[engine.getCurrentImageIndex()], sizeof(LightUniformBuffer));
        lightBuffer->flush();

        constants.transform = flatVase.transform.mat4();
        constants.normal = flatVase.transform.normal();
        commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, constants);
        drawModel(commandBuffer, flatVase);

        constants.transform = smoothVase.transform.mat4();
        constants.normal = smoothVase.transform.normal();
        commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, constants);
        drawModel(commandBuffer, smoothVase);

        constants.transform = floor.transform.mat4();
        constants.normal = floor.transform.normal();
        commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, constants);
        drawModel(commandBuffer, floor);
    }

    void MainSystem::createDescriptorLayout() {
        std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {};
        coffee::DescriptorBindingInfo binding {};

        binding.type = coffee::DescriptorType::UniformBuffer;
        binding.stages = coffee::ShaderStage::All;
        bindings[0] = binding;

        binding.type = coffee::DescriptorType::UniformBuffer;
        binding.stages = coffee::ShaderStage::Fragment;
        bindings[1] = binding;

        layout = engine.createDescriptorLayout(bindings);
    }

    void MainSystem::createBuffers() {
        coffee::BufferConfiguration configuration {};
        configuration.usage = coffee::BufferUsage::Uniform;
        configuration.properties = coffee::MemoryProperty::HostVisible | coffee::MemoryProperty::HostCoherent;
        configuration.instanceCount = 1U;

        configuration.instanceSize = sizeof(MVPUniformBuffer);
        mvpBuffer = engine.createBuffer(configuration);

        configuration.instanceSize = sizeof(LightUniformBuffer);
        lightBuffer = engine.createBuffer(configuration);
    }

    void MainSystem::createDescriptorSet() {
        descriptorSet = engine.createDescriptorSet(
            coffee::DescriptorWriter(layout)
                .addBuffer(0, mvpBuffer)
                .addBuffer(1, lightBuffer));
    }

    void MainSystem::createPipeline(const coffee::RenderPass& renderPass) {
        coffee::PushConstants pushConstants {};

        pushConstants
            .addRange(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment)
            .addObject<MainPushConstants>();

        coffee::ShaderProgram program {};
        program.addVertexShader(engine.createShader("shaders/base.vert.spv", coffee::ShaderStage::Vertex));
        program.addFragmentShader(engine.createShader("shaders/base.frag.spv", coffee::ShaderStage::Fragment));

        coffee::PipelineConfiguration configuration {};

        configuration.inputAssembly.topology = coffee::Topology::Triangle;
        configuration.inputAssembly.primitiveRestartEnable = false;

        configuration.rasterizationInfo.fillMode = coffee::PolygonMode::Solid;
        configuration.rasterizationInfo.cullMode = coffee::CullMode::None;
        configuration.rasterizationInfo.frontFace = coffee::FrontFace::Clockwise;
        configuration.rasterizationInfo.depthBiasEnable = false;
        configuration.rasterizationInfo.depthBiasConstantFactor = 0.0f;
        configuration.rasterizationInfo.depthBiasClamp = 0.0f;
        configuration.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

        configuration.multisampleInfo.sampleRateShading = false;
        configuration.multisampleInfo.sampleCount = 1U;
        configuration.multisampleInfo.minSampleShading = 1.0f;
        configuration.multisampleInfo.alphaToCoverage = false;

        configuration.colorBlendAttachment.blendEnable = false;
        configuration.colorBlendAttachment.colorWriteMask =
            coffee::ColorComponent::Red   |
            coffee::ColorComponent::Green |
            coffee::ColorComponent::Blue  |
            coffee::ColorComponent::Alpha ;
        configuration.colorBlendAttachment.srcBlend = coffee::BlendFactor::One;
        configuration.colorBlendAttachment.dstBlend = coffee::BlendFactor::Zero;
        configuration.colorBlendAttachment.blendOp = coffee::BlendOp::Add;
        configuration.colorBlendAttachment.alphaSrcBlend = coffee::BlendFactor::One;
        configuration.colorBlendAttachment.alphaDstBlend = coffee::BlendFactor::Zero;
        configuration.colorBlendAttachment.alphaBlendOp = coffee::BlendOp::Add;
        configuration.colorBlendAttachment.logicOpEnable = false;
        configuration.colorBlendAttachment.logicOp = coffee::LogicOp::Copy;

        configuration.depthStencilInfo.depthTestEnable = true;
        configuration.depthStencilInfo.depthWriteEnable = true;
        configuration.depthStencilInfo.depthCompareOp = coffee::CompareOp::Less;
        configuration.depthStencilInfo.stencilTestEnable = false;
        configuration.depthStencilInfo.frontFace = {};
        configuration.depthStencilInfo.backFace = {};

        configuration.inputBindings.push_back(
            { 0U, sizeof(Model::Vertex), coffee::InputRate::PerVertex, Model::Vertex::getElementDescriptions() });

        pipeline = engine.createPipeline(renderPass, pushConstants, { layout }, program, configuration);
    }

    std::shared_ptr<Model> MainSystem::createModel(const std::string& filePath) {
        return std::make_shared<Model>(engine, filePath);
    }

    void MainSystem::drawModel(const coffee::CommandBuffer& commandBuffer, const ModelObject& model) {
        commandBuffer->bindVertexBuffer(model.model_->vertexBuffer);

        if (model.model_->indexBuffer == nullptr) {
            commandBuffer->draw(model.model_->vertexCount);
            return;
        }

        commandBuffer->bindIndexBuffer(model.model_->indexBuffer);
        commandBuffer->drawIndexed(model.model_->indexCount);
    }

}