#include <editor/editor.hpp>
#include <editor/imgui.hpp>
#include <editor/timestamps.hpp>

#include <yorna/yorna.hpp>

#include <oneapi/tbb/task_group.h>

int main() {
    auto gpuDevice = coffee::graphics::Device::create();
    auto loopHandler = coffee::LoopHandler::create();

    auto window = coffee::graphics::Window::create(gpuDevice, {
        .extent = { 1280, 720 },
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
        .hiddenOnStart = true,
        .fullscreen = false
    });

    yorna::Yorna gameHandler { gpuDevice, loopHandler };
    yorna::Editor editor { gpuDevice, gameHandler };
    yorna::ImGuiImplementation imguiHandler { gpuDevice, window };
    yorna::QueryTimestamps timestamps { gpuDevice, 2 };

    auto gameThreadWork = [&gpuDevice, &gameHandler, &timestamps]() {
        yorna::ImGuiBackendPlatformData* platformData = static_cast<yorna::ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        coffee::graphics::CommandBuffer commandBuffer = coffee::graphics::CommandBuffer::createGraphics(gpuDevice);
 
        timestamps.resetQueryPool(commandBuffer);
        gameHandler.bindWindow(platformData->fullControlWindowPtr);
        gameHandler.update();

        timestamps.writeBeginTimestamp(commandBuffer, 0);
        gameHandler.performDepthTest(commandBuffer);
        timestamps.writeEndTimestamp(commandBuffer, 0);

        timestamps.writeBeginTimestamp(commandBuffer, 1);
        gameHandler.performRendering(commandBuffer);
        timestamps.writeEndTimestamp(commandBuffer, 1);

        gpuDevice->sendCommandBuffer(std::move(commandBuffer));
    };

    tbb::task_group gameLoopTask {};

    loopHandler.setFramerateLimit(1440.0f);
    window->showWindow();

    while (!window->shouldClose()) {
        loopHandler.pollEvents();

        while (!imguiHandler.isAnyWindowActive()) {
            loopHandler.pollEvents(1.0 / 60.0);
        }

        auto loopBeginTime = std::chrono::high_resolution_clock::now();
        float deltaTime = loopHandler.deltaTime();

        imguiHandler.update(deltaTime);
        gameLoopTask.run(gameThreadWork);
        editor.render();
        imguiHandler.render();

        auto imGuiTime = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - loopBeginTime).count();

        gameLoopTask.wait();
        gpuDevice->submitPendingWork();

        editor.updateAverageTimings(deltaTime, imGuiTime, timestamps);

        loopHandler.waitFramelimit();
    }

    return 0;
}