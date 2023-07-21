#ifndef YORNA_GAME
#define YORNA_GAME

#include <coffee/graphics/window.hpp>
#include <coffee/interfaces/asset_manager.hpp>
#include <coffee/interfaces/loop_handler.hpp>

#include <array>
#include <atomic>

#include <yorna/forward_plus.hpp>
#include <yorna/shared_instance.hpp>
#include <yorna/sunlight_shadow.hpp>

#include <entities/camera.hpp>
#include <entities/model.hpp>

namespace yorna {

    constexpr size_t kMaxAmountOfPointLights = 64U;
    constexpr size_t kMaxAmountOfSpotLights = 4U;

    class Yorna : SharedInstance {
    public:
        Yorna(const SharedInstance& instance, coffee::LoopHandler& loopHandler);
        ~Yorna();

        void bindWindow(const coffee::graphics::Window* window) noexcept;
        void update();

        void cullMeshes();
        void performDepthTest(const coffee::graphics::CommandBuffer& commandBuffer);
        void submitDepthTest(coffee::graphics::CommandBuffer&& commandBuffer);
        void performLightCulling(const coffee::graphics::CommandBuffer& commandBuffer);
        void submitLightCulling(coffee::graphics::CommandBuffer&& commandBuffer);
        void performRendering(const coffee::graphics::CommandBuffer& commandBuffer);
        void submitRendering(coffee::graphics::CommandBuffer&& commandBuffer);

        std::atomic<float> outputAspect { 1280.0f / 720.0f };

        coffee::graphics::DescriptorLayoutPtr outputLayout;
        coffee::graphics::DescriptorSetPtr outputDescriptor;

        Camera camera {};
        TransformComponent viewerObject {};

        void nextFrame() noexcept;
        uint32_t currentFrame() const noexcept;

    private:
        void createBuffers();
        void createSamplers();
        void createDescriptors();
        void createSyncObjects();
        void loadModels();

        // Must be recreated when window resizes or when present mode changes
        void updateDescriptors();

        void updateCamera(const coffee::graphics::CommandBuffer& commandBuffer);
        void updateObjects(const coffee::graphics::CommandBuffer& commandBuffer);
        void updateLights(const coffee::graphics::CommandBuffer& commandBuffer);

        void mousePositionCallback(const coffee::graphics::Window& window, const coffee::MouseMoveEvent& e) noexcept;
        void keyCallback(const coffee::graphics::Window& window, const coffee::KeyEvent& e) noexcept;

        coffee::LoopHandler& loopHandler;
        const coffee::graphics::Window* boundWindow = nullptr;

        EarlyDepth earlyDepth;
        SunlightShadow sunlightShadow;
        LightCulling lightCulling;
        ForwardPlus forwardPlus;

        coffee::graphics::BufferPtr pointLights;
        coffee::graphics::BufferPtr spotLights;

        PerFlightFrame<coffee::graphics::SemaphorePtr> updateUBOSemaphores;
        PerFlightFrame<coffee::graphics::SemaphorePtr> earlyDepthSemaphores;
        PerFlightFrame<coffee::graphics::SemaphorePtr> lightCullingSemaphores;
        PerFlightFrame<coffee::graphics::FencePtr> completionFences;

        PerFlightFrame<coffee::graphics::SubmitSemaphores> submitUpdateUBOSemaphores;
        PerFlightFrame<coffee::graphics::SubmitSemaphores> submitEarlyDepthSemaphores;
        PerFlightFrame<coffee::graphics::SubmitSemaphores> submitLightCullingSemaphores;
        PerFlightFrame<coffee::graphics::SubmitSemaphores> submitRenderingSemaphores;

        float lookSpeed = 0.003f;
        float moveSpeed = 10.0f;
        std::unique_ptr<Model> model;

        coffee::graphics::SamplerPtr textureSampler;
        coffee::graphics::SamplerPtr outputSampler;

        uint32_t frameIndex = 0;
    };

} // namespace yorna

#endif