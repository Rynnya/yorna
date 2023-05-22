#include <systems/imgui/system.hpp>

int main() {
    auto device = coffee::GPUDevice::create();

    coffee::LoopHandler loopHandler {};
    editor::ImGuiSystem imguiSystem { device, loopHandler };

    imguiSystem.run();

    return 0;
}