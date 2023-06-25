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

        void removeChilds(entt::entity childEntity);
        void drawHierarchyTree(entt::entity entity);

        coffee::graphics::DevicePtr device;
        yorna::Yorna& gameHandle;

        ProjectInformation projectInformation {};
        HierarchyViewport sceneHierarchy {};
        SceneViewport sceneViewport {};
        AssetBrowserViewport assetBrowser {};
    };

}

#endif