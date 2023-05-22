#ifndef SYSTEMS_SANDBOX_SYSTEM
#define SYSTEMS_SANDBOX_SYSTEM

#include <coffee/coffee.hpp>
#include <coffee/interfaces/asset_manager.hpp>

#include <atomic>

#include <systems/sandbox/structures.hpp>

#include <entities/camera.hpp>
#include <entities/model.hpp>

namespace editor {

    class MainSystem {
    public:
        MainSystem(const coffee::GPUDevicePtr& device, coffee::LoopHandler& loopHandler);
        ~MainSystem();

        void bindWindow(const coffee::Window* window) noexcept;
        void unbindWindow() noexcept;

        void update();

        void performDepthTest(const coffee::CommandBuffer& commandBuffer);
        void performRendering(const coffee::CommandBuffer& commandBuffer);

        std::atomic<float> outputAspect { 1280.0f / 720.0f };

        coffee::DescriptorLayoutPtr outputLayout;
        coffee::DescriptorSetPtr outputSet;

        Camera camera {};
        TransformComponent viewerObject {};

    private:
        void createSamplers();
        void createBuffers();
        void loadModels();
        void createRenderPasses();
        void createPipelines();

        // Must be recreated when window resizes or when present mode changes
        void createImages();
        void createFramebuffer();
        void updateDescriptors();

        void createDescriptors();

        void updateCamera();
        void updateObjects();
        void updateLightPoints();

        void mousePositionCallback(const coffee::Window& window, const coffee::MouseMoveEvent& e) noexcept;
        void keyCallback(const coffee::Window& window, const coffee::KeyEvent& e) noexcept;

        const coffee::GPUDevicePtr& device;
        coffee::LoopHandler& loopHandler;

        coffee::AssetManagerPtr assetManager;
        coffee::FilesystemPtr filesystem;

        const coffee::Window* boundWindow = nullptr;

        float lookSpeed = 0.003f;
        float moveSpeed = 10.0f;
        UModel sponzaModel;

        std::vector<PointLight> pointLights {};

        MainPushConstants mainConstants {};
        LightPushConstants lightPointsConstants {};

        coffee::RenderPassPtr earlyDepthPass;
        coffee::RenderPassPtr renderPass;
        coffee::PipelinePtr earlyDepthPipeline;
        coffee::PipelinePtr mainPipeline;
        coffee::PipelinePtr lightPointsPipeline;

        coffee::ImagePtr colorImage;
        coffee::ImagePtr depthImage;
        coffee::ImageViewPtr colorImageView;
        coffee::ImageViewPtr depthImageView;
        coffee::FramebufferPtr earlyDepthFramebuffer;
        coffee::FramebufferPtr framebuffer;

        coffee::SamplerPtr textureSampler;
        coffee::SamplerPtr outputSampler;

        coffee::DescriptorLayoutPtr gameLayout;

        std::vector<FrameInfo> frameInfos {};
        uint32_t frameInfoIndex = 0;
    };

}

#endif