#ifndef YORNA_EDITOR_IMGUI
#define YORNA_EDITOR_IMGUI

#include <coffee/graphics/window.hpp>
#include <coffee/interfaces/loop_handler.hpp>

#include <entt.hpp>
#include <imgui.h>

namespace yorna {

    struct ImGuiPushConstant {
        glm::vec2 scale {};
        glm::vec2 translate {};
    };

    struct ImGuiBackendPlatformData {
        const coffee::graphics::Window* windowPtr = nullptr;
        // This flag must be set to window than took full control over mouse
        // Basically means when window->disableCursor() called
        const coffee::graphics::Window* fullControlWindowPtr = nullptr;
        const coffee::graphics::Window* keyOwnerWindows[static_cast<uint32_t>(coffee::Keys::Last)]{};
        coffee::graphics::CursorPtr cursors[ImGuiMouseCursor_COUNT] {};
        ImVec2 lastMousePos{};
        bool wantUpdateMonitors = false;
    };

    struct ImGuiBackendRendererData {
        coffee::graphics::DevicePtr device = nullptr;
        coffee::graphics::DescriptorLayoutPtr layout = nullptr;
        coffee::graphics::DescriptorSetPtr descriptorSet = nullptr;
        coffee::graphics::RenderPassPtr renderPass = nullptr;
        coffee::graphics::PipelinePtr pipeline = nullptr;
        coffee::graphics::ImagePtr fonts = nullptr;
        coffee::graphics::ImageViewPtr fontsView = nullptr;
        coffee::graphics::SamplerPtr fontsSampler = nullptr;
    };

    struct ImGuiViewportData {
        coffee::graphics::Window* windowHandle;
        int32_t ignoreWindowPositionFrameCount = 0;
        int32_t ignoreWindowSizeFrameCount = 0;
    };

    struct ImGuiRendererData {
        std::vector<coffee::graphics::ImageViewPtr> swapChainViews {};
        std::vector<coffee::graphics::FramebufferPtr> framebuffers {};
        std::vector<coffee::graphics::BufferPtr> vertexBuffers {};
        std::vector<coffee::graphics::BufferPtr> indexBuffers {};
    };

    class ImGuiImplementation {
    public:
        ImGuiImplementation(const coffee::graphics::DevicePtr& device, const coffee::graphics::WindowPtr& applicationWindow);
        ~ImGuiImplementation();

        void update(float deltaTime);
        void render();

        bool isAnyWindowActive();

    private:
        static void render(ImGuiViewport* viewport, const coffee::graphics::CommandBuffer& commandBuffer);

        void initializeImGui();
        void initializeBackend();

        void createFonts();
        void createDescriptors();
        void createRenderPass();
        void createPipeline();

        static void focusCallback(const coffee::graphics::Window& window, const coffee::WindowFocusEvent& e);
        static void enterCallback(const coffee::graphics::Window& window, const coffee::WindowEnterEvent& e);
        static void mouseClickCallback(const coffee::graphics::Window& window, const coffee::MouseClickEvent& e);
        static void mousePositionCallback(const coffee::graphics::Window& window, const coffee::MouseMoveEvent& e);
        static void mouseWheelCallback(const coffee::graphics::Window& window, const coffee::MouseWheelEvent& e);
        static void keyCallback(const coffee::graphics::Window& window, const coffee::KeyEvent& e);
        static void charCallback(const coffee::graphics::Window& window, char32_t ch);
        static void updateMonitors();

        void updateMouse(const coffee::graphics::WindowPtr& window);
        void updateCursor(const coffee::graphics::WindowPtr& window);

        const coffee::graphics::DevicePtr& device;
        const coffee::graphics::WindowPtr& applicationWindow;

        bool acquired = false;
    };

}

#endif