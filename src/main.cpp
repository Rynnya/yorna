#include <coffee/engine.hpp>

#include "systems/imgui_behaviour.hpp"
#include "systems/main_behaviour.hpp"
#include "systems/light_behaviour.hpp"

int main() {
    // Initialize window with 1280x720 resolution
    coffee::WindowSettings windowSettings {};
    windowSettings.width = 1280U;
    windowSettings.height = 720U;

    // Initialize engine which will create a Vulkan backend and window through GLFW
    coffee::Engine engine { coffee::BackendSelect::Vulkan, windowSettings };
    // Also create arrays for swap chain images
    std::vector<coffee::Framebuffer> swapChainFramebuffers {};

    // We also need a framebuffer for game renderers
    coffee::Sampler gameSampler = nullptr;
    coffee::Image gameColorImage = nullptr;
    coffee::Image gameDepthImage = nullptr;
    coffee::Framebuffer gameFramebuffer = nullptr;
    coffee::DescriptorSet gameDescriptorSet = nullptr;

    coffee::DescriptorLayout gameDescriptorLayout = [&]() {
        std::map<uint32_t, coffee::DescriptorBindingInfo> bindingInfo {};

        bindingInfo[0].type = coffee::DescriptorType::ImageAndSampler;
        bindingInfo[0].stages = coffee::ShaderStage::Fragment;

        return engine.createDescriptorLayout(bindingInfo);
    }();

    coffee::RenderPassConfiguration renderPassConfiguration {};

    auto& colorAttachment = renderPassConfiguration.colorAttachments.emplace_back();
    colorAttachment.sampleCount = 1U;
    colorAttachment.initialState = coffee::ResourceState::Undefined;
    colorAttachment.finalState = coffee::ResourceState::Present;
    colorAttachment.format = engine.getColorFormat();
    colorAttachment.loadOp = coffee::AttachmentLoadOp::Clear;
    colorAttachment.storeOp = coffee::AttachmentStoreOp::Store;
    colorAttachment.stencilLoadOp = coffee::AttachmentLoadOp::DontCare;
    colorAttachment.stencilStoreOp = coffee::AttachmentStoreOp::DontCare;
    colorAttachment.clearValue = std::array<float, 4> { 0.01f, 0.01f, 0.01f, 1.0f };

    // Interface will be used by ImGUI
    coffee::RenderPass interfaceRenderPass = engine.createRenderPass(renderPassConfiguration);

    coffee::AttachmentConfiguration depthStencilAttachment {};
    depthStencilAttachment.initialState = coffee::ResourceState::Undefined;
    depthStencilAttachment.finalState = coffee::ResourceState::DepthWrite;
    depthStencilAttachment.format = engine.getDepthFormat();
    depthStencilAttachment.loadOp = coffee::AttachmentLoadOp::Clear;
    depthStencilAttachment.storeOp = coffee::AttachmentStoreOp::DontCare;
    depthStencilAttachment.stencilLoadOp = coffee::AttachmentLoadOp::DontCare;
    depthStencilAttachment.stencilStoreOp = coffee::AttachmentStoreOp::DontCare;
    depthStencilAttachment.depthStencilClearValue = coffee::ClearDepthStencilValue { 1.0f, 0U };

    colorAttachment.finalState = coffee::ResourceState::ShaderResource;
    renderPassConfiguration.depthStencilAttachment = depthStencilAttachment;

    // And game one will be used by Main and Light systems
    coffee::RenderPass gameRenderPass = engine.createRenderPass(renderPassConfiguration);

    coffee::SamplerConfiguration samplerConfiguration {};
    samplerConfiguration.magFilter = coffee::SamplerFilter::Linear;
    samplerConfiguration.minFilter = coffee::SamplerFilter::Linear;
    samplerConfiguration.mipmapMode = coffee::SamplerFilter::Linear;
    samplerConfiguration.addressModeU = coffee::AddressMode::ClampToEdge;
    samplerConfiguration.addressModeV = coffee::AddressMode::ClampToEdge;
    samplerConfiguration.addressModeW = coffee::AddressMode::ClampToEdge;
    samplerConfiguration.mipLodBias = 0.0f;
    samplerConfiguration.maxAnisotropy = 0U;
    samplerConfiguration.compareOp = coffee::CompareOp::Never;
    samplerConfiguration.borderColor = coffee::BorderColor::OpaqueWhite;
    samplerConfiguration.minLod = 0.0f;
    samplerConfiguration.maxLod = 1.0f;
    gameSampler = engine.createSampler(samplerConfiguration);

    // A lambda that will recreate images, which allows easy way to resize image
    auto createFramebuffers = [&]() {
        swapChainFramebuffers.clear();

        const auto& presentImages = engine.getPresentImages();

        for (size_t i = 0; i < presentImages.size(); i++) {
            coffee::FramebufferConfiguration framebufferConfiguration {};
            framebufferConfiguration.width = engine.getFramebufferWidth();
            framebufferConfiguration.height = engine.getFramebufferHeight();
            framebufferConfiguration.colorViews.push_back(presentImages[i]);
            swapChainFramebuffers.push_back(engine.createFramebuffer(interfaceRenderPass, framebufferConfiguration));
        }

        coffee::ImageConfiguration colorImageConfiguration {};

        colorImageConfiguration.type = coffee::ImageType::TwoDimensional;
        colorImageConfiguration.format = engine.getColorFormat();
        colorImageConfiguration.width = engine.getFramebufferWidth();
        colorImageConfiguration.height = engine.getFramebufferHeight();
        colorImageConfiguration.usage = coffee::ImageUsage::ColorAttachment | coffee::ImageUsage::Sampled;
        colorImageConfiguration.initialState = coffee::ResourceState::Undefined;
        colorImageConfiguration.viewType = coffee::ImageViewType::TwoDimensional;
        colorImageConfiguration.aspects = coffee::ImageAspect::Color;
        gameColorImage = engine.createImage(colorImageConfiguration);

        colorImageConfiguration.format = engine.getDepthFormat();
        colorImageConfiguration.usage = coffee::ImageUsage::DepthStencilAttachment;
        colorImageConfiguration.aspects = coffee::ImageAspect::Depth;
        gameDepthImage = engine.createImage(colorImageConfiguration);

        coffee::FramebufferConfiguration framebufferConfiguration {};
        framebufferConfiguration.width = engine.getFramebufferWidth();
        framebufferConfiguration.height = engine.getFramebufferHeight();
        framebufferConfiguration.colorViews.push_back(gameColorImage);
        framebufferConfiguration.depthStencilView = gameDepthImage;
        gameFramebuffer = engine.createFramebuffer(gameRenderPass, framebufferConfiguration);

        if (gameDescriptorSet == nullptr) {
            coffee::DescriptorWriter writer = coffee::DescriptorWriter(gameDescriptorLayout);
            writer.addImage(0, coffee::ResourceState::ShaderResource, gameColorImage, gameSampler);

            gameDescriptorSet = engine.createDescriptorSet(writer);
            return;
        }

        gameDescriptorSet->updateDescriptor(
            coffee::DescriptorWriter(gameDescriptorLayout).addImage(0, coffee::ResourceState::ShaderResource, gameColorImage, gameSampler));
    };

    engine.addWindowResizeCallback("framebuffersRecreateOnResize", [&](const coffee::ResizeEvent& resizeEvent) {
        createFramebuffers();
    });

    engine.addPresentModeCallback("gameFramebufferPresentModeChanged", [&](const coffee::PresentModeEvent&) {
        createFramebuffers();
    });

    createFramebuffers();

    game::ImGuiSystem imguiSystem { engine, interfaceRenderPass, gameDescriptorSet };
    game::MainSystem mainSystem { engine, gameRenderPass };
    game::LightPointSystem lightSystem { engine, gameRenderPass };

    engine.setFrameLimit(60.0f);
    engine.showWindow();

    while (!engine.shouldExit()) {
        engine.pollEvents();

        if (engine.acquireFrame()) {
            coffee::Framebuffer& framebuffer = swapChainFramebuffers[engine.getCurrentImageIndex()];

            coffee::CommandBuffer interfaceCommandBuffer = engine.createCommandBuffer();
            coffee::CommandBuffer gameCommandBuffer = engine.createCommandBuffer();

            interfaceCommandBuffer->beginRenderPass(
                interfaceRenderPass, framebuffer, { engine.getFramebufferWidth(), engine.getFramebufferHeight() });

            imguiSystem.update();
            imguiSystem.render(interfaceCommandBuffer);

            gameCommandBuffer->beginRenderPass(
                gameRenderPass, gameFramebuffer, { engine.getFramebufferWidth(), engine.getFramebufferHeight() });

            mainSystem.update();
            lightSystem.update();

            mainSystem.render(gameCommandBuffer);
            lightSystem.render(gameCommandBuffer);

            engine.sendCommandBuffer(std::move(gameCommandBuffer));
            engine.sendCommandBuffer(std::move(interfaceCommandBuffer));

            engine.endFrame();
        }

        engine.wait();
    }

    return 0;
}