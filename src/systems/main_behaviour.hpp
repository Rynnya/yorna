#ifndef GAME_MAIN_BEHAVIOUR
#define GAME_MAIN_BEHAVIOUR

#include <coffee/engine.hpp>

#include "../game_objects/camera.hpp"
#include "../game_objects/game_object.hpp"
#include "../game_objects/light_object.hpp"
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
        };

        struct MVPUniformBuffer {
            glm::mat4 projection {};
            glm::mat4 view {};
            glm::mat4 inverseView {};
            glm::vec4 ambientLightColor { 1.0f, 1.0f, 1.0f, 0.1f };
        };

        struct LightPoint {
            glm::vec4 position {};
            glm::vec4 color {};
        };

        struct LightUniformBuffer {
            LightPoint lightPoints[10] {};
            uint32_t size {};
        };

        MainSystem(coffee::Window& window);
        ~MainSystem();

        void performDepthPass(const coffee::CommandBuffer& commandBuffer);
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

        float lookSpeed = 0.003f;
        float moveSpeed = 10.0f;
        UModel sponzaModel, backpackModel;

        std::vector<LightObject> lights {};

        MainPushConstants mainConstants {};
        LightPushConstants lightPointsConstants {};

        coffee::RenderPass earlyDepthPass;
        coffee::RenderPass renderPass;
        coffee::Pipeline earlyDepthPipeline;
        coffee::Pipeline mainPipeline;
        coffee::Pipeline lightPointsPipeline;

        coffee::Image colorImage;
        coffee::Image depthImage;
        coffee::Framebuffer earlyDepthFramebuffer;
        coffee::Framebuffer framebuffer;
        coffee::DescriptorLayout layout;

        coffee::Sampler textureSampler;
        coffee::Sampler outputSampler;

        struct FrameInfo {
            MVPUniformBuffer mvpUbo {};
            LightUniformBuffer lightUbo {};

            coffee::DescriptorSet descriptorSet;
            coffee::Buffer mvpBuffer;
            coffee::Buffer lightBuffer;
        };

        std::vector<FrameInfo> frameInfos {};
        coffee::Window& window;
    };

}

#endif