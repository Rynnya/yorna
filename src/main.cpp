#include <systems/imgui/system.hpp>
#include <systems/sandbox/system.hpp>

#include <coffee/audio/device.hpp>
#include <coffee/audio/source.hpp>

#if defined(COFFEE_EDITOR_ENABLED)

int main() {
    auto gpuDevice = coffee::graphics::Device::create();
    auto loopHandler = coffee::LoopHandler::create();

    loopHandler.setFramerateLimit(1440.0f);

    editor::ImGuiSystem imguiSystem { gpuDevice, loopHandler };
    imguiSystem.run();

    return 0;
}

#else

int main() {
    auto gpuDevice = coffee::graphics::Device::create();
    auto loopHandler = coffee::LoopHandler::create();

    auto applicationWindow = coffee::graphics::Window::create(gpuDevice, {
        .extent = { 1280U, 720U },
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .hiddenOnStart = true
    });

    editor::MainSystem sandboxSystem { gpuDevice, loopHandler };

    loopHandler.setFramerateLimit(60.0f);
    sandboxSystem.bindWindow(applicationWindow.get());
    applicationWindow->showWindow();

    while (!applicationWindow->shouldClose()) {
        loopHandler.pollEvents();

        while (applicationWindow->isIconified()) {
            loopHandler.pollEvents(1.0 / 60.0);
        }

        if (applicationWindow->acquireNextImage()) {
            auto commandBuffer = coffee::graphics::CommandBuffer::createGraphics(gpuDevice);

            sandboxSystem.update();
            sandboxSystem.performDepthTest(commandBuffer);
            sandboxSystem.performRendering(commandBuffer);

            // TODO: Currently MainSystem doesn't render to swap chain images
            // I have 2 options how to deal with such issue:
            // 1 - Create dummy swap chain that will only do gamma correction
            // 2 - Do everything in MainSystem
            // After some thinking I will implement one of them

            applicationWindow->sendCommandBuffer(std::move(commandBuffer));
            gpuDevice->submitPendingWork();
        }

        loopHandler.waitFramelimit();
    }

    sandboxSystem.unbindWindow();

    return 0;
}
#endif