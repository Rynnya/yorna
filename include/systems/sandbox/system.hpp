#ifndef SYSTEMS_SANDBOX_SYSTEM
#define SYSTEMS_SANDBOX_SYSTEM

#include <coffee/coffee.hpp>
#include <coffee/interfaces/asset_manager.hpp>

#include <array>
#include <atomic>

#include <systems/shadows/sunlight_shadow.hpp>
#include <systems/sandbox/structures.hpp>

#include <entities/camera.hpp>
#include <entities/model.hpp>

namespace editor {

    class MainSystem {
    public:
        MainSystem(const coffee::graphics::DevicePtr& device, coffee::LoopHandler& loopHandler);
        ~MainSystem();

        void bindWindow(const coffee::graphics::Window* window) noexcept;
        void unbindWindow() noexcept;

        void update();

        void cullMeshes();
        void performDepthTest(const coffee::graphics::CommandBuffer& commandBuffer);
        void performRendering(const coffee::graphics::CommandBuffer& commandBuffer);

        coffee::graphics::ImagePtr& image();

        std::atomic<float> outputAspect { 1280.0f / 720.0f };

        coffee::graphics::DescriptorLayoutPtr outputLayout;
        coffee::graphics::DescriptorSetPtr outputSet;

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

        void mousePositionCallback(const coffee::graphics::Window& window, const coffee::MouseMoveEvent& e) noexcept;
        void keyCallback(const coffee::graphics::Window& window, const coffee::KeyEvent& e) noexcept;

        const coffee::graphics::DevicePtr& device;
        coffee::LoopHandler& loopHandler;

        coffee::AssetManagerPtr assetManager;
        coffee::FilesystemPtr filesystem;

        SunLightShadow sunlightShadow;

        const coffee::graphics::Window* boundWindow = nullptr;

        float lookSpeed = 0.003f;
        float moveSpeed = 10.0f;
        UModel sponzaModel;

        std::vector<PointLight> pointLights {};
        std::vector<SpotLight> spotLights {};

        MainPushConstants mainConstants {};
        LightPushConstants lightPointsConstants {};

        coffee::graphics::RenderPassPtr earlyDepthPass;
        coffee::graphics::RenderPassPtr renderPass;
        coffee::graphics::PipelinePtr earlyDepthPipeline;
        coffee::graphics::PipelinePtr mainPipeline;
        coffee::graphics::PipelinePtr lightPointsPipeline;

        coffee::graphics::ImagePtr colorImage;
        coffee::graphics::ImagePtr depthImage;
        coffee::graphics::ImageViewPtr colorImageView;
        coffee::graphics::ImageViewPtr depthImageView;
        coffee::graphics::FramebufferPtr earlyDepthFramebuffer;
        coffee::graphics::FramebufferPtr framebuffer;

        coffee::graphics::SamplerPtr textureSampler;
        coffee::graphics::SamplerPtr outputSampler;

        coffee::graphics::DescriptorLayoutPtr gameLayout;

        std::array<FrameInfo, coffee::graphics::Device::kMaxOperationsInFlight> frameInfos {};
        uint32_t frameInfoIndex = 0;
    };

}

#endif