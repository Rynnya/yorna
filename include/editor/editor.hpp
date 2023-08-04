#ifndef YORNA_EDITOR
#define YORNA_EDITOR

#include <editor/components.hpp>
#include <editor/entities.hpp>
#include <editor/timestamps.hpp>
#include <editor/viewports.hpp>

#include <yorna/yorna.hpp>

#include <coffee/graphics/descriptors.hpp>

namespace yorna {

    class Editor : SharedInstance {
    public:
        struct MousePickingPushConstants {
            glm::mat4 modelMatrix;
            uint32_t objectID;
        };

        struct UpdateInformation {
            bool modelWasChanged = false;
            glm::mat4 transformationMatrix { 1.0f };
        };

        Editor(const SharedInstance& instance, Yorna& gameHandle) noexcept;
        ~Editor() noexcept = default;

        // Changes state in single-threaded mode so no race condition is done
        void update();
        // Should not change game state directly, instead use update() method
        void render();
        void updateAverageTimings(float deltaTime, float imguiTiming, QueryTimestamps& queryTimestamps);

    private:
        void initializeLights();
        void initializeMemoryHeaps();
        void initializeMousePicking();

        void resizeMousePicking();

        void createProject();
        void loadProject();
        void loadAssetDirectory();
        void saveProject();

        std::string_view hierarchyTypeToString(HierarchyType type);
        [[nodiscard]] entt::entity insertNewRootNode(HierarchyType type);
        [[nodiscard]] entt::entity insertNewChildNode(entt::entity parentEntity, HierarchyComponent& parentComponent, HierarchyType type);

        void removeChilds(entt::entity childEntity);
        void drawHierarchyTree(entt::entity entity);

        void doMousePicking(const ImVec2& currentCursorPosition);
        void boundViewportToGame(ImGuiViewport* viewport);
        void unboundViewportFromGame(ImGuiViewport* viewport);

        void acquireModelIndex(entt::entity entity);
        void releaseModelIndex(entt::entity entity);
        void acquirePointLightIndex(entt::entity entity);
        void releasePointLightIndex(entt::entity entity);
        void acquireSpotLightIndex(entt::entity entity);
        void releaseSpotLightIndex(entt::entity entity);

        void outputFramerateInfo(const std::string_view& name, const float (&buffer)[SceneViewport::kAverageStatisticBufferSize]);
        void outputMemoryUsage(const std::array<VmaBudget, VK_MAX_MEMORY_HEAPS>& budgets, const std::string_view& name, int32_t heapIndex);

        yorna::Yorna& gameHandle;
        coffee::graphics::DescriptorSetPtr framebufferImage {};
        UpdateInformation updateInformation {};

        ProjectInformation projectInformation {};
        HierarchyViewport hierarchy {};
        GizmoViewport gizmo {};
        SceneViewport scene {};
        AssetBrowserViewport assetBrowser {};

        ModelLoader modelLoader;

        std::array<bool, Yorna::kMaxAmountOfPointLights> freePointLightIndices {};
        std::array<bool, Yorna::kMaxAmountOfSpotLights> freeSpotLightIndices {};
        uint32_t pointLightCount = 0;
        uint32_t spotLightCount = 0;

        constexpr static float kDefaultAspectRatio = 16.0f / 9.0f;
    };

} // namespace yorna

#endif