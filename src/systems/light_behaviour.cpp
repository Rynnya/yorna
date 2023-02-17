#include "light_behaviour.hpp"

#include <coffee/engine.hpp>

#include "structs.hpp"

namespace game {

    std::vector<glm::vec3> lightColors {
        {1.f, .1f, .1f},
        {.1f, .1f, 1.f},
        {.1f, 1.f, .1f},
        {1.f, 1.f, .1f},
        {.1f, 1.f, 1.f},
        {1.f, 1.f, 1.f} 
    };

    LightPointSystem::LightPointSystem(
        coffee::Engine& engine,
        const coffee::RenderPass& renderPass
    ) : engine { engine } {
        createDescriptorLayout();
        createBuffers();
        createDescriptorSet();
        createPipeline(renderPass);

        for (size_t i = 0; i < lightColors.size(); i++) {
            LightObject& object = lights.emplace_back(LightObject::createLightObject(0.2f));
            object.color = lightColors[i];
            auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, (i * glm::two_pi<float>()) / lightColors.size(), { 0.0f, -1.0f, 0.0f });
            object.transform.translation = glm::vec3(rotateLight * glm::vec4 { -1.0f, -1.0f, -1.0f, 1.0f });
        }
    }

    LightPointSystem::~LightPointSystem() {
        engine.waitDeviceIdle();
    }

    void LightPointSystem::update() {
        auto rotateLight = glm::rotate(glm::mat4 { 1.0f }, engine.getDeltaTime(), { 0.0f, -1.0f, 0.0f });
        LightUniformBuffer& currentLightBuffer = StaticObjects::lightUniformBuffers[engine.getCurrentImageIndex()];

        for (size_t i = 0; i < lights.size(); i++) {
            lights[i].transform.translation = glm::vec3(rotateLight * glm::vec4 { lights[i].transform.translation, 1.0f });

            currentLightBuffer.lightPoints[i].position = glm::vec4(lights[i].transform.translation, 1.0f);
            currentLightBuffer.lightPoints[i].color = glm::vec4(lights[i].color, lights[i].intensity);
        }

        currentLightBuffer.length = static_cast<uint32_t>(lights.size());
    }

    void LightPointSystem::render(const coffee::CommandBuffer& commandBuffer) {
        LightUniformBuffer& currentLightBuffer = StaticObjects::lightUniformBuffers[engine.getCurrentImageIndex()];

        commandBuffer->bindDescriptorSets(pipeline, descriptorSet);
        commandBuffer->bindPipeline(pipeline);
        commandBuffer->setViewport(engine.getFramebufferWidth(), engine.getFramebufferHeight());
        commandBuffer->setScissor(engine.getFramebufferWidth(), engine.getFramebufferHeight());

        std::memcpy(mvpBuffer->map(), &StaticObjects::mvpUniformBuffers[engine.getCurrentImageIndex()], sizeof(MVPUniformBuffer));
        mvpBuffer->flush();
        std::memcpy(lightBuffer->map(), &currentLightBuffer, sizeof(LightUniformBuffer));
        lightBuffer->flush();

        for (size_t i = 0; i < lights.size(); i++) {
            constants.position = currentLightBuffer.lightPoints[i].position;
            constants.color = currentLightBuffer.lightPoints[i].color;
            constants.radius = lights[i].transform.scale.r;

            commandBuffer->pushConstants(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment, constants);
            commandBuffer->draw(6);
        }
    }

    void LightPointSystem::createDescriptorLayout() {
        std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {};
        coffee::DescriptorBindingInfo binding{};

        binding.type = coffee::DescriptorType::UniformBuffer;
        binding.stages = coffee::ShaderStage::All;
        bindings[0] = binding;

        binding.type = coffee::DescriptorType::UniformBuffer;
        binding.stages = coffee::ShaderStage::Fragment;
        bindings[1] = binding;

        layout = engine.createDescriptorLayout(bindings);
    }

    void LightPointSystem::createBuffers() {
        coffee::BufferConfiguration configuration {};
        configuration.usage = coffee::BufferUsage::Uniform;
        configuration.properties = coffee::MemoryProperty::HostVisible | coffee::MemoryProperty::HostCoherent;
        configuration.instanceCount = 1U;

        configuration.instanceSize = sizeof(MVPUniformBuffer);
        mvpBuffer = engine.createBuffer(configuration);

        configuration.instanceSize = sizeof(LightUniformBuffer);
        lightBuffer = engine.createBuffer(configuration);
    }

    void LightPointSystem::createDescriptorSet() {
        descriptorSet = engine.createDescriptorSet(
            coffee::DescriptorWriter(layout)
                .addBuffer(0, mvpBuffer)
                .addBuffer(1, lightBuffer));
    }

    void LightPointSystem::createPipeline(const coffee::RenderPass& renderPass) {
        coffee::PushConstants pushConstants {};

        pushConstants
            .addRange(coffee::ShaderStage::Vertex | coffee::ShaderStage::Fragment)
            .addObject<LightPushConstants>();

        coffee::ShaderProgram program {};
        program.addVertexShader(engine.createShader("shaders/point_light.vert.spv", coffee::ShaderStage::Vertex));
        program.addFragmentShader(engine.createShader("shaders/point_light.frag.spv", coffee::ShaderStage::Fragment));

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

        pipeline = engine.createPipeline(renderPass, pushConstants, { layout }, program, configuration);
    }

}