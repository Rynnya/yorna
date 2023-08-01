#include <editor/editor.hpp>
#include <editor/imgui.hpp>
#include <editor/timestamps.hpp>

#include <yorna/yorna.hpp>

#include <oneapi/tbb/task_group.h>

int main()
{
    auto gpuDevice = coffee::graphics::Device::create({ .fragmentStoresAndAtomics = true });
    yorna::SharedInstance sharedInstance { gpuDevice };

    auto loopHandler = coffee::LoopHandler::create();
    auto window = coffee::graphics::Window::create(gpuDevice, {
        .extent = { .width = 1280, .height = 720, },
        .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
        .hiddenOnStart = true,
        .fullscreen = false,
    });

    yorna::Yorna gameHandler { sharedInstance, loopHandler };
    yorna::Editor editor { sharedInstance, gameHandler };
    yorna::ImGuiImplementation imguiHandler { gpuDevice, window };
    yorna::QueryTimestamps timestamps { gpuDevice, 3 };

    auto gameThreadWork = [&gpuDevice, &gameHandler, &timestamps]() {
        auto earlyDepthCommandBuffer = coffee::graphics::CommandBuffer::createGraphics(gpuDevice);
        auto lightCullingCommandBuffer = coffee::graphics::CommandBuffer::createCompute(gpuDevice);
        auto renderingCommandBuffer = coffee::graphics::CommandBuffer::createGraphics(gpuDevice);

        timestamps.resetQueryPool(earlyDepthCommandBuffer);

        timestamps.writeBeginTimestamp(earlyDepthCommandBuffer, 0);
        gameHandler.performDepthTest(earlyDepthCommandBuffer);
        timestamps.writeEndTimestamp(earlyDepthCommandBuffer, 0);
        gameHandler.submitDepthTest(std::move(earlyDepthCommandBuffer));

        timestamps.writeBeginTimestamp(lightCullingCommandBuffer, 1);
        gameHandler.performLightCulling(lightCullingCommandBuffer);
        timestamps.writeEndTimestamp(lightCullingCommandBuffer, 1);
        gameHandler.submitLightCulling(std::move(lightCullingCommandBuffer));

        timestamps.writeBeginTimestamp(renderingCommandBuffer, 2);
        gameHandler.performRendering(renderingCommandBuffer);
        timestamps.writeEndTimestamp(renderingCommandBuffer, 2);
        gameHandler.submitRendering(std::move(renderingCommandBuffer));

        gameHandler.nextFrame();
    };

    tbb::task_group gameLoopTask {};

    loopHandler.setFramerateLimit(1400.0f);
    window->showWindow();

    while (!window->shouldClose()) {
        loopHandler.pollEvents();

        while (!imguiHandler.isAnyWindowActive()) {
            loopHandler.pollEvents(1.0 / 60.0);
        }

        auto loopBeginTime = std::chrono::high_resolution_clock::now();
        float deltaTime = loopHandler.deltaTime();

        yorna::ImGuiBackendPlatformData* platformData = static_cast<yorna::ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);

        imguiHandler.update(deltaTime);
        editor.update();
        gameHandler.bindWindow(platformData->fullControlWindowPtr);
        gameHandler.update();

        gameLoopTask.run(gameThreadWork);
        editor.render();
        imguiHandler.render();

        auto imGuiTime = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - loopBeginTime).count();

        gameLoopTask.wait();
        gpuDevice->present();

        editor.updateAverageTimings(deltaTime, imGuiTime, timestamps);

        loopHandler.waitFramelimit();
    }

    return 0;
}