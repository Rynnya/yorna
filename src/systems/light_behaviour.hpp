#ifndef GAME_LIGHT_BEHAVIOUR
#define GAME_LIGHT_BEHAVIOUR

#include "../game_objects/light_object.hpp"
#include "structs.hpp"

namespace game {

    class LightPointSystem {
    public:
        LightPointSystem(
            coffee::Engine& engine,
            const coffee::RenderPass& renderPass);
        ~LightPointSystem();

        struct LightPushConstants {
            glm::vec4 position {};
            glm::vec4 color {};
            float radius {};
        };

        void update();
        void render(const coffee::CommandBuffer& commandBuffer);

    private:
        void createDescriptorLayout();
        void createBuffers();
        void createDescriptorSet();
        void createPipeline(const coffee::RenderPass& renderPass);

        std::vector<LightObject> lights;
        LightPushConstants constants {};

        coffee::DescriptorLayout layout;
        coffee::DescriptorSet descriptorSet;
        coffee::Pipeline pipeline;

        coffee::Buffer mvpBuffer;
        coffee::Buffer lightBuffer;

        coffee::Engine& engine;
    };

}

#endif