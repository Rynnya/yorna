#ifndef GAME_MAIN_BEHAVIOUR
#define GAME_MAIN_BEHAVIOUR

#include <coffee/engine.hpp>

#include "../game_objects/camera.hpp"
#include "../game_objects/game_object.hpp"
#include "../game_objects/model.hpp"

namespace game {

    class MainSystem {
    public:
        struct MainPushConstants {
            glm::mat4 transform { 1.0f };
            glm::mat4 normal { 1.0f };
        };

        struct LightPushConstants {
            glm::vec4 position {};
            glm::vec4 color {};
            float radius {};
        };

        MainSystem(coffee::Engine& engine);
        ~MainSystem();

        void performDepthPass();
        void updateObjects();
        void updateLightPoints();
        void renderObjects(const coffee::CommandBuffer& commandBuffer);
        void renderLightPoints(const coffee::CommandBuffer& commandBuffer);

        void beginRenderPass(const coffee::CommandBuffer& commandBuffer);
        void endRenderPass(const coffee::CommandBuffer& commandBuffer);

        coffee::DescriptorLayout outputLayout;
        coffee::DescriptorSet outputSet;

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

        Camera camera {};
        GameObject viewerObject { GameObject::createGameObject() };

        const float lookSpeed = 0.13f;
        const float moveSpeed = 10.0f;
        UModel sponzaModel, backpackModel;

        MainPushConstants mainConstants {};
        LightPushConstants lightPointsConstants {};

        coffee::RenderPass renderPass;
        coffee::Pipeline mainPipeline;
        coffee::Pipeline lightPointsPipeline;

        coffee::Image colorImage;
        coffee::Image depthImage;
        coffee::Framebuffer framebuffer;
        coffee::DescriptorLayout layout;
        coffee::DescriptorSet descriptorSet;

        coffee::Buffer mvpBuffer;
        coffee::Buffer lightBuffer;
        coffee::Sampler textureSampler;
        coffee::Sampler outputSampler;

        coffee::Engine& engine;
    };

}

#endif