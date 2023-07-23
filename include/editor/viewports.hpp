#ifndef YORNA_EDITOR_VIEWPORTS
#define YORNA_EDITOR_VIEWPORTS

#include <editor/entities.hpp>
#include <entities/transform_component.hpp>

#include <ImGuizmo.h>
#include <coffee/interfaces/filesystem.hpp>
#include <imgui_file_dialog.h>

#include <filesystem>

namespace yorna {

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
    static constexpr T calculateAverage(const T (&buffer)[Size])
    {
        T result {};

        for (size_t i = 0; i < Size; i++) {
            result += buffer[i];
        }

        return (result / static_cast<T>(Size));
    }

    struct SceneViewport : Viewport {
        static constexpr uint32_t kAverageStatisticBufferSize = 32;
        static constexpr int32_t kInvalidHeapIndex = -1;

        bool keepAspectRatio = false;
        bool outputFramerateAndFPS = false;

        float averageFrameTime[kAverageStatisticBufferSize] {};
        float averageFPS[kAverageStatisticBufferSize] {};

        float averageImGuiTime[kAverageStatisticBufferSize] {};
        float averageDepthPass[kAverageStatisticBufferSize] {};
        float averageLightPass[kAverageStatisticBufferSize] {};
        float averageRendering[kAverageStatisticBufferSize] {};

        uint32_t statisticIndex = 0;
        int32_t deviceHeapIndex = kInvalidHeapIndex;
        int32_t hostHeapIndex = kInvalidHeapIndex;
        int32_t sharedHeapIndex = kInvalidHeapIndex;

        ImVec2 size {};
        ImVec2 framerateTextPosition {};
    };

    struct GuizmoViewport : Viewport {
        ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
        ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;
        TransformComponent* affectedModel = nullptr;
    };

    struct DirectoryObject {
        enum class Type { Unknown = 0, Directory = 1, Mesh = 2, Texture = 3, Shader = 4, CompiledShader = 5, Audio = 6 } type = Type::Unknown;

        std::filesystem::path relativePath {};
        std::u8string filename {};
        bool selected = false;
    };

    struct AssetBrowserViewport : Viewport {
        bool updateRequired = false;
        std::filesystem::path rootFolder {};
        std::filesystem::path currentFolder {};
        std::vector<DirectoryObject> content {};
    };

} // namespace yorna

#endif
