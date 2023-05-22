#ifndef SYSTEMS_IMGUI_VIEWPORTS
#define SYSTEMS_IMGUI_VIEWPORTS

#include <systems/imgui/entities.hpp>

#include <imgui_file_dialog.h>

#include <filesystem>

namespace editor {

    struct ProjectInformation {
        ImGuiFileDialog dialog {};
        bool loaded = false;
    };

    struct Viewport {
        bool active = false;
    };

    struct HierarchyViewport : Viewport {
        entt::registry components {};
        entt::entity renameEntity = entt::null;
        entt::entity deleteEntity = entt::null;
    };

    template <typename T, size_t Size>
    static constexpr T calculateAverage(const T(&buffer)[Size]) {
        T result {};

        for (size_t i = 0; i < Size; i++) {
            result += buffer[i];
        }

        return (result / static_cast<T>(Size));
    }

    struct SceneViewport : Viewport {
        static constexpr uint32_t averageStatisticBufferSize = 32;
        static constexpr int32_t invalidHeapIndex = -1;

        bool keepAspectRatio = false;
        bool outputFramerateAndFPS = false;

        float averageFrameTime[averageStatisticBufferSize] {};
        float averageFPS[averageStatisticBufferSize] {};

        float averageCPUTime[averageStatisticBufferSize] {};
        float averageDepthPass[averageStatisticBufferSize] {};
        float averageRendering[averageStatisticBufferSize] {};

        uint32_t statisticIndex = 0;
        int32_t deviceHeapIndex = invalidHeapIndex;
        int32_t hostHeapIndex = invalidHeapIndex;
        int32_t sharedHeapIndex = invalidHeapIndex;
    };

    struct DirectoryObject {

        enum class Type {
            Unknown = 0,
            Directory = 1,
            Mesh = 2,
            Texture = 3
        };

        std::filesystem::path relativePath {};
        std::u8string filename {};
        bool selected = false;
        Type type = Type::Unknown;
    };

    struct AssetBrowserViewport : Viewport {
        bool updateRequired = false;
        std::filesystem::path rootFolder {};
        std::filesystem::path currentFolder {};
        std::vector<DirectoryObject> content {};
    };

}

#endif
