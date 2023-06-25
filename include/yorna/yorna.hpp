#ifndef YORNA_GAME
#define YORNA_GAME

#include <coffee/graphics/window.hpp>
#include <coffee/interfaces/asset_manager.hpp>
#include <coffee/interfaces/loop_handler.hpp>

#include <array>
#include <atomic>

#include <yorna/shadows/sunlight_shadow.hpp>

#include <entities/camera.hpp>
#include <entities/model.hpp>

namespace yorna {

    constexpr size_t kMaxAmountOfPointLights = 64;
    constexpr size_t kMaxAmountOfSpotLights = 32;

    struct MainPushConstants {
        glm::mat4 transform { 1.0f };
        glm::mat4 normal { 1.0f };
    };

    struct LightPushConstants {
        glm::vec4 position {};
        glm::vec4 color {};
    };

    struct MVPUniformBuffer {
        glm::mat4 projection {};
        glm::mat4 view {};
        glm::mat4 inverseView {};
        glm::vec4 ambientLightColor { 1.0f, 1.0f, 1.0f, 0.07f };
    };

    struct PointLightUBO {
        glm::vec4 position {};
        glm::vec4 color {};
    };

    struct SpotLightUBO {
        glm::vec4 position {};
        glm::vec4 color {};
        glm::vec4 coneDirection {}; // w is coneAngle
    };

    struct DirectionalLightUBO {
        glm::vec4 direction{};
        glm::vec4 color{};
    };

    struct LightUniformBuffer {
        glm::mat4 sunlightSpaceMatrix { 1.0f };
        glm::vec4 sunlightDirection { 1.0f };
        glm::vec4 sunlightColor { 1.0f };
        uint32_t amountOfPointLights {};
        uint32_t amountOfSpotLights {};
        alignas(16) PointLightUBO pointLights[kMaxAmountOfPointLights] {};
        SpotLightUBO spotLights[kMaxAmountOfSpotLights] {};
    };

    static_assert(offsetof(LightUniformBuffer, pointLights) == sizeof(glm::mat4) + 3 * sizeof(glm::vec4), "Invalid alignment");

    struct FrameInfo {
        MVPUniformBuffer mvpUbo {};
        LightUniformBuffer lightUbo {};

        coffee::graphics::DescriptorSetPtr descriptorSet;
        coffee::graphics::BufferPtr mvpBuffer;
        coffee::graphics::BufferPtr lightBuffer;
    };

    class Yorna {
    public:
        Yorna(const coffee::graphics::DevicePtr& device, coffee::LoopHandler& loopHandler);
        ~Yorna();

        void bindWindow(const coffee::graphics::Window* window) noexcept;
        void update();

        void cullMeshes();
        void performDepthTest(const coffee::graphics::CommandBuffer& commandBuffer);
        void performRendering(const coffee::graphics::CommandBuffer& commandBuffer);

        std::atomic<float> outputAspect { 1280.0f / 720.0f };

        coffee::graphics::ImagePtr colorImage;
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
        std::unique_ptr<Model> sponzaModel;

        std::vector<PointLight> pointLights {};
        std::vector<SpotLight> spotLights {};

        MainPushConstants mainConstants {};
        LightPushConstants lightPointsConstants {};

        coffee::graphics::RenderPassPtr earlyDepthPass;
        coffee::graphics::RenderPassPtr renderPass;
        coffee::graphics::PipelinePtr earlyDepthPipeline;
        coffee::graphics::PipelinePtr mainPipeline;
        coffee::graphics::PipelinePtr lightPointsPipeline;

        coffee::graphics::ImagePtr depthImage;
        coffee::graphics::ImageViewPtr colorImageView;
        coffee::graphics::ImageViewPtr depthImageView;
        coffee::graphics::FramebufferPtr earlyDepthFramebuffer;
        coffee::graphics::FramebufferPtr framebuffer;

        coffee::graphics::SamplerPtr textureSampler;
        coffee::graphics::SamplerPtr outputSampler;

        coffee::graphics::DescriptorLayoutPtr gameLayout;

        std::array<FrameInfo, coffee::graphics::Device::kMaxOperationsInFlight> frameInfos{};
        uint32_t frameInfoIndex = 0;
    };

}

#endif