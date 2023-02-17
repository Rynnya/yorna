#ifndef GAME_MAIN_BEHAVIOUR
#define GAME_MAIN_BEHAVIOUR

#include <coffee/engine.hpp>

#include "../game_objects/camera.hpp"
#include "../game_objects/model_object.hpp"

namespace game {

    class MainSystem {
        friend class Model;
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
        void createDescriptorLayout();
        void createBuffers();
        void createDescriptorSet();
        void createPipeline(const coffee::RenderPass& renderPass);

        std::shared_ptr<Model> createModel(const std::string& filePath);
        static void drawModel(const coffee::CommandBuffer& commandBuffer, const ModelObject& model);

        Camera camera {};
        GameObject viewerObject { GameObject::createGameObject() };

        const float lookSpeed = 0.13f;
        const float moveSpeed = 2.0f;
        ModelObject flatVase, smoothVase, floor;

        MainPushConstants constants {};

        coffee::DescriptorLayout layout;
        coffee::DescriptorSet descriptorSet;
        coffee::Pipeline pipeline;

        coffee::Buffer mvpBuffer;
        coffee::Buffer lightBuffer;

        coffee::Engine& engine;
    };

}

#endif