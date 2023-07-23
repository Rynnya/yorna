#ifndef YORNA_EDITOR
#define YORNA_EDITOR

#include <editor/entities.hpp>
#include <editor/timestamps.hpp>
#include <editor/viewports.hpp>

#include <yorna/yorna.hpp>

#include <coffee/graphics/descriptors.hpp>

namespace yorna {

    class Editor {
    public:
        Editor(const coffee::graphics::DevicePtr& device, yorna::Yorna& gameHandle) noexcept;
        ~Editor() noexcept = default;

        void render();
        void updateAverageTimings(float deltaTime, float imguiTiming, QueryTimestamps& queryTimestamps);

        coffee::graphics::DescriptorSetPtr framebufferImage = nullptr;

    private:
        void createProject();
        void loadProject();
        void loadAssetDirectory();
        void saveProject();

        std::string_view hierarchyTypeToString(HierarchyType type);
        [[nodiscard]] entt::entity insertNewRootNode(HierarchyType type);
        [[nodiscard]] entt::entity insertNewChildNode(entt::entity parentEntity, HierarchyComponent& parentComponent, HierarchyType type);

        void removeChilds(entt::entity childEntity);
        void drawHierarchyTree(entt::entity entity);

        void acquirePointLightIndex(entt::entity entity);
        void releasePointLightIndex(entt::entity entity);
        void acquireSpotLightIndex(entt::entity entity);
        void releaseSpotLightIndex(entt::entity entity);

        void outputFramerateInfo(const std::string_view& name, const float (&buffer)[SceneViewport::kAverageStatisticBufferSize]);
        void outputMemoryUsage(const std::array<VmaBudget, VK_MAX_MEMORY_HEAPS>& budgets, const std::string_view& name, int32_t heapIndex);

        coffee::graphics::DevicePtr device;
        yorna::Yorna& gameHandle;

        ProjectInformation projectInformation {};
        HierarchyViewport sceneHierarchy {};
        GuizmoViewport guizmoViewport {};
        SceneViewport sceneViewport {};
        AssetBrowserViewport assetBrowser {};

        std::array<bool, Yorna::kMaxAmountOfPointLights> freePointLightIndices {};
        std::array<bool, Yorna::kMaxAmountOfSpotLights> freeSpotLightIndices {};
        uint32_t pointLightCount = 0;
        uint32_t spotLightCount = 0;

        constexpr static float kDefaultAspectRatio = 16.0f / 9.0f;
    };

} // namespace yorna

#endif