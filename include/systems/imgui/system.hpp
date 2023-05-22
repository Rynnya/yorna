#ifndef SYSTEMS_IMGUI_SYSTEM
#define SYSTEMS_IMGUI_SYSTEM

#include <coffee/graphics/descriptors.hpp>
#include <coffee/graphics/window.hpp>
#include <coffee/interfaces/loop_handler.hpp>
#include <coffee/interfaces/thread_pool.hpp>

#include <systems/imgui/viewports.hpp>

namespace editor {

    class ImGuiSystem {
    public:
        ImGuiSystem(const coffee::GPUDevicePtr& device, coffee::LoopHandler& loopHandler);
        ~ImGuiSystem();

        void run();

        void update(float deltaTime);
        void render();

    private:
        static void render(ImGuiViewport* viewport, const coffee::CommandBuffer& commandBuffer);

        void initializeImGui();
        void initializeBackend();

        void createFonts();
        void createDescriptors();
        void createRenderPass();
        void createPipeline();

        static void focusCallback(const coffee::Window& window, const coffee::WindowFocusEvent& e);
        static void enterCallback(const coffee::Window& window, const coffee::WindowEnterEvent& e);
        static void mouseClickCallback(const coffee::Window& window, const coffee::MouseClickEvent& e);
        static void mousePositionCallback(const coffee::Window& window, const coffee::MouseMoveEvent& e);
        static void mouseWheelCallback(const coffee::Window& window, const coffee::MouseWheelEvent& e);
        static void keyCallback(const coffee::Window& window, const coffee::KeyEvent& e);
        static void charCallback(const coffee::Window& window, char32_t ch);
        static void updateMonitors();

        void updateMouse(const coffee::WindowPtr& window);
        void updateCursor(const coffee::WindowPtr& window);

        void createQueryPool();
        void destroyQueryPool();
        void resetQueryPool(const coffee::CommandBuffer& commandBuffer);
        void beginTimestamp(const coffee::CommandBuffer& commandBuffer, uint32_t index);
        void endTimestamp(const coffee::CommandBuffer& commandBuffer, uint32_t index);
        float getTimestampResult(uint32_t index);

        bool isAnyWindowActive();

        void createProject();
        void loadProject();
        void loadAssetDirectory();
        void saveProject();

        void removeChilds(entt::entity childEntity);
        void drawHierarchyTree(entt::entity entity);

        void prepareImGui();

        const coffee::GPUDevicePtr& device;
        coffee::LoopHandler& loopHandler;

        coffee::WindowPtr applicationWindow;
        bool acquired = false;
        coffee::DescriptorSetPtr framebufferImage;
        coffee::ThreadPool gameThreadPool;

        VkQueryPool timestampQueryPool = VK_NULL_HANDLE;

        ProjectInformation projectInformation {};

        HierarchyViewport sceneHierarchy {};
        SceneViewport sceneViewport {};
        AssetBrowserViewport assetBrowser {};
    };

}

#endif