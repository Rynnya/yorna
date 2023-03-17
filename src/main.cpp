#include <coffee/engine.hpp>

#include <coffee/interfaces/thread_pool.hpp>

#include "systems/imgui_behaviour.hpp"
#include "systems/main_behaviour.hpp"
#include "systems/structs.hpp"

int main() {
    coffee::Engine::initialize(coffee::BackendAPI::Vulkan);

    coffee::ThreadPool threadPool { 2U };

    // Initialize window with 1280x720 resolution
    coffee::WindowSettings windowSettings {};
    windowSettings.extent.width = 1280U;
    windowSettings.extent.height = 720U;

    {
        coffee::Window window = coffee::Factory::createWindow(windowSettings);

        game::ImGuiSystem imguiSystem { window };
        //game::MainSystem mainSystem { window };
        //imguiSystem.framebufferImage = mainSystem.outputSet;

        //auto gameThread = [&factory, &mainSystem]() {
        //    coffee::CommandBuffer commandBuffer = factory.createCommandBuffer();

        //    mainSystem.updateObjects();
        //    mainSystem.updateLightPoints();

        //    mainSystem.performDepthPass(commandBuffer);
        //    mainSystem.beginRenderPass(commandBuffer);
        //    {
        //        mainSystem.renderObjects(commandBuffer);
        //        mainSystem.renderLightPoints(commandBuffer);
        //    }
        //    mainSystem.endRenderPass(commandBuffer);

        //    factory.sendCommandBuffer(std::move(commandBuffer));
        //};

        coffee::Engine::setFramerateLimit(60.0f);
        window->showWindow();

        std::vector<coffee::CommandBuffer> commandBuffers {};

        while (!window->shouldClose()) {
            coffee::Engine::pollEvents();

            imguiSystem.update();
            imguiSystem.render();

            coffee::Engine::wait();
        }
    }

    coffee::Engine::destroy();

    return 0;
}