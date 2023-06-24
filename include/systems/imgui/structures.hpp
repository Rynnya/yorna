#ifndef SYSTEMS_IMGUI_STRUCTURES
#define SYSTEMS_IMGUI_STRUCTURES

#include <coffee/coffee.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <systems/sandbox/system.hpp>

#include <imgui.h>
#include <imgui_file_dialog.h>

namespace editor {

    struct ImGuiPushConstant {
        glm::vec2 scale {};
        glm::vec2 translate {};
    };

    struct BackendPlatformData {
        const coffee::graphics::Window* windowPtr = nullptr;
        // This flag must be set to window than took full control over mouse
        // Basically means when window->disableCursor() called
        const coffee::graphics::Window* fullControlWindowPtr = nullptr;
        const coffee::graphics::Window* keyOwnerWindows[static_cast<uint32_t>(coffee::Keys::Last)] {};
        coffee::graphics::CursorPtr cursors[ImGuiMouseCursor_COUNT] {};
        ImVec2 lastMousePos {};
        bool wantUpdateMonitors = false;

        std::unique_ptr<editor::MainSystem> gameSystem = nullptr;
    };

    struct BackendRendererData {
        coffee::graphics::DevicePtr device = nullptr;
        coffee::graphics::DescriptorLayoutPtr layout = nullptr;
        coffee::graphics::DescriptorSetPtr descriptorSet = nullptr;
        coffee::graphics::RenderPassPtr renderPass = nullptr;
        coffee::graphics::PipelinePtr pipeline = nullptr;
        coffee::graphics::ImagePtr fonts = nullptr;
        coffee::graphics::ImageViewPtr fontsView = nullptr;
        coffee::graphics::SamplerPtr fontsSampler = nullptr;
    };

    struct ViewportData {
        coffee::graphics::Window* windowHandle;
        int32_t ignoreWindowPositionFrameCount = 0;
        int32_t ignoreWindowSizeFrameCount = 0;
    };

    struct RendererData {
        std::vector<coffee::graphics::ImageViewPtr> swapChainViews {};
        std::vector<coffee::graphics::FramebufferPtr> framebuffers {};
        std::vector<coffee::graphics::BufferPtr> vertexBuffers {};
        std::vector<coffee::graphics::BufferPtr> indexBuffers {};
    };

}

#endif
