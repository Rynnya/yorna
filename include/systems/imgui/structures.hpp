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
        const coffee::Window* windowPtr = nullptr;
        // This flag must be set to window than took full control over mouse
        // Basically means when window->disableCursor() called
        const coffee::Window* fullControlWindowPtr = nullptr;
        const coffee::Window* keyOwnerWindows[static_cast<uint32_t>(coffee::Keys::Last)] {};
        coffee::CursorPtr cursors[ImGuiMouseCursor_COUNT] {};
        ImVec2 lastMousePos {};
        bool wantUpdateMonitors = false;

        std::unique_ptr<editor::MainSystem> gameSystem = nullptr;
    };

    struct BackendRendererData {
        coffee::GPUDevicePtr device = nullptr;
        coffee::DescriptorLayoutPtr layout = nullptr;
        coffee::DescriptorSetPtr descriptorSet = nullptr;
        coffee::RenderPassPtr renderPass = nullptr;
        coffee::PipelinePtr pipeline = nullptr;
        coffee::ImagePtr fonts = nullptr;
        coffee::ImageViewPtr fontsView = nullptr;
        coffee::SamplerPtr fontsSampler = nullptr;
    };

    struct ViewportData {
        coffee::Window* windowHandle;
        int32_t ignoreWindowPositionFrameCount = 0;
        int32_t ignoreWindowSizeFrameCount = 0;
    };

    struct RendererData {
        std::vector<coffee::ImageViewPtr> swapChainViews {};
        std::vector<coffee::FramebufferPtr> framebuffers {};
        std::vector<coffee::BufferPtr> vertexBuffers {};
        std::vector<coffee::BufferPtr> indexBuffers {};
    };

}

#endif
