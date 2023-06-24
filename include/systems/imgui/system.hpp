#ifndef SYSTEMS_IMGUI_SYSTEM
#define SYSTEMS_IMGUI_SYSTEM

#include <coffee/graphics/descriptors.hpp>
#include <coffee/graphics/window.hpp>
#include <coffee/interfaces/loop_handler.hpp>

#include <systems/imgui/viewports.hpp>

namespace editor {

    class ImGuiSystem {
    public:
        ImGuiSystem(const coffee::graphics::DevicePtr& device, coffee::LoopHandler& loopHandler);
        ~ImGuiSystem();

        void run();

        void update(float deltaTime);
        void render();

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

        void createQueryPool();
        void destroyQueryPool();
        void resetQueryPool(const coffee::graphics::CommandBuffer& commandBuffer);
        void beginTimestamp(const coffee::graphics::CommandBuffer& commandBuffer, uint32_t index);
        void endTimestamp(const coffee::graphics::CommandBuffer& commandBuffer, uint32_t index);
        void writeTimestampResults();

        bool isAnyWindowActive();

        void createProject();
        void loadProject();
        void loadAssetDirectory();
        void saveProject();

        void removeChilds(entt::entity childEntity);
        void drawHierarchyTree(entt::entity entity);

        void prepareImGui();

        const coffee::graphics::DevicePtr& device;
        coffee::LoopHandler& loopHandler;

        bool acquired = false;
        coffee::graphics::WindowPtr applicationWindow;
        coffee::graphics::DescriptorSetPtr framebufferImage;

        VkQueryPool timestampQueryPool = VK_NULL_HANDLE;
        std::vector<uint32_t> beginTimestampWrites {};
        std::vector<uint32_t> endTimestampWrites {};

        ProjectInformation projectInformation {};

        HierarchyViewport sceneHierarchy {};
        SceneViewport sceneViewport {};
        AssetBrowserViewport assetBrowser {};
    };

}

#endif