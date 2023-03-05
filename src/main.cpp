#include <coffee/engine.hpp>

#include "systems/imgui_behaviour.hpp"
#include "systems/main_behaviour.hpp"

int main() {
    // Initialize window with 1280x720 resolution
    coffee::WindowSettings windowSettings {};
    windowSettings.width = 1280U;
    windowSettings.height = 720U;

    // Initialize engine which will create a Vulkan backend and window through GLFW
    coffee::Engine engine { coffee::BackendSelect::Vulkan, windowSettings };

    game::MainSystem mainSystem { engine };
    game::ImGuiSystem imguiSystem { engine };
    imguiSystem.framebufferImage = mainSystem.outputSet;

    engine.setFrameLimit(60.0f);
    engine.showWindow();

    while (!engine.shouldExit()) {
        engine.pollEvents();

        if (engine.acquireFrame()) {
            coffee::CommandBuffer interfaceCommandBuffer = engine.createCommandBuffer();
            coffee::CommandBuffer gameCommandBuffer = engine.createCommandBuffer();

            imguiSystem.update();
            imguiSystem.render(interfaceCommandBuffer);

            mainSystem.updateObjects();
            mainSystem.updateLightPoints();

            mainSystem.performDepthPass(gameCommandBuffer);
            mainSystem.beginRenderPass(gameCommandBuffer);
            {
                mainSystem.renderObjects(gameCommandBuffer);
                mainSystem.renderLightPoints(gameCommandBuffer);
            }
            mainSystem.endRenderPass(gameCommandBuffer);

            engine.sendCommandBuffer(std::move(gameCommandBuffer));
            engine.sendCommandBuffer(std::move(interfaceCommandBuffer));

            engine.endFrame();
        }

        engine.wait();
    }

    return 0;
}