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

        struct BackendPlatformData {
            const coffee::AbstractWindow* windowPtr = nullptr;
            const coffee::AbstractWindow* keyOwnerWindows[static_cast<uint32_t>(coffee::Keys::Last)] {};
            ImVec2 lastMousePos {};
            bool wantUpdateMonitors = false;
        };

        struct BackendRendererData {
            coffee::DescriptorLayout layout = nullptr;
            coffee::DescriptorSet descriptorSet = nullptr;
            coffee::RenderPass renderPass = nullptr;
            coffee::Pipeline pipeline = nullptr;
            coffee::Image fonts = nullptr;
            coffee::Sampler fontsSampler = nullptr;
        };

        struct ViewportData {
            coffee::AbstractWindow* windowHandle;
            int32_t ignoreWindowPositionFrameCount = 0;
            int32_t ignoreWindowSizeFrameCount = 0;
        };

        struct RendererData {
            std::vector<coffee::Framebuffer> framebuffers {};
            std::vector<coffee::Buffer> vertexBuffers {};
            std::vector<coffee::Buffer> indexBuffers {};
        };

        ImGuiSystem(coffee::Window& window);
        ~ImGuiSystem();

        void update();
        void render();

        coffee::DescriptorSet framebufferImage;

    private:
        static void render(ImGuiViewport* viewport, const coffee::CommandBuffer& commandBuffer);

        void initializeImGui();
        void initializeBackend();

        void createFonts();
        void createDescriptors();
        void createRenderPass();
        void createPipeline();

        static void focusCallback(const coffee::AbstractWindow* const window, const coffee::WindowFocusEvent& e);
        static void enterCallback(const coffee::AbstractWindow* const window, const coffee::WindowEnterEvent& e);
        static void mouseClickCallback(const coffee::AbstractWindow* const window, const coffee::MouseClickEvent& e);
        static void mousePositionCallback(const coffee::AbstractWindow* const window, const coffee::MouseMoveEvent& e);
        static void mouseWheelCallback(const coffee::AbstractWindow* const window, const coffee::MouseWheelEvent& e);
        static void keyCallback(const coffee::AbstractWindow* const window, const coffee::KeyEvent& e);
        static void charCallback(const coffee::AbstractWindow* const window, char32_t ch);
        static void updateMonitors();

        void updateMouse(const coffee::Window& window);
        void updateCursor(const coffee::Window& window);

        void prepareImGui(const coffee::Window& window);

        coffee::Window& mainWindow;
    };

}

#endif