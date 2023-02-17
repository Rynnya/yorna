#ifndef GAME_IMGUI_BEHAVIOUR
#define GAME_IMGUI_BEHAVIOUR

#include <coffee/engine.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <imgui.h>

namespace game {

    class ImGuiSystem {
    public:

        struct ImGuiPushConstant {
            glm::vec2 scale;
            glm::vec2 translate;
        };

        ImGuiSystem(coffee::Engine& engine, const coffee::RenderPass& renderPass, coffee::DescriptorSet& image);
        ~ImGuiSystem();

        void update();
        void render(const coffee::CommandBuffer& commandBuffer);

    private:
        void createFonts();
        void createDescriptorLayout();
        void createDescriptorSet();
        void createPipeline(const coffee::RenderPass& renderPass);

        void updateMouse();
        void updateCursor();

        void prepareImGui();

        ImVec2 lastMousePos {};
        ImGuiPushConstant constants {};

        coffee::DescriptorLayout layout;
        coffee::DescriptorSet descriptorSet;
        coffee::Pipeline pipeline;

        coffee::Image fonts;
        coffee::Sampler fontsSampler;
        std::vector<coffee::Buffer> vertexBuffers;
        std::vector<coffee::Buffer> indexBuffers;

        coffee::Engine& engine;
        coffee::DescriptorSet& image;
    };

}

#endif