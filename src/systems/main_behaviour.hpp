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

        MainSystem(
            coffee::Engine& engine,
            const coffee::RenderPass& renderPass);
        ~MainSystem();

        void update();
        void render(const coffee::CommandBuffer& commandBuffer);

    private:
        void loadModels();
        void createDescriptors();
        void createBuffers();
        void createTextureSampler();
        void createPipeline(const coffee::RenderPass& renderPass);

        Camera camera {};
        GameObject viewerObject { GameObject::createGameObject() };

        const float lookSpeed = 0.13f;
        const float moveSpeed = 10.0f;
        UModel sponzaModel, backpackModel;

        MainPushConstants constants {};

        coffee::DescriptorLayout layout;
        coffee::DescriptorSet descriptorSet;
        coffee::DescriptorSet nullTextureDescriptor;
        coffee::Pipeline pipeline;

        coffee::Buffer mvpBuffer;
        coffee::Buffer lightBuffer;
        coffee::Sampler textureSampler;

        coffee::Engine& engine;
    };

}

#endif