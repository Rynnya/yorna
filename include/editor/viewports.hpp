#ifndef YORNA_EDITOR_VIEWPORTS
#define YORNA_EDITOR_VIEWPORTS

#include <editor/entities.hpp>
#include <entities/transform_component.hpp>
#include <yorna/shared_instance.hpp>

#include <coffee/graphics/descriptors.hpp>
#include <coffee/graphics/framebuffer.hpp>
#include <coffee/graphics/graphics_pipeline.hpp>
#include <coffee/graphics/render_pass.hpp>
#include <coffee/interfaces/filesystem.hpp>

#include <Imguizmo.h>
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

        bool cameraCaptured = false;
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

    struct GizmoViewport : Viewport {
        ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;

        TransformComponent* affectedModel = nullptr;
        bool waitingForModelSelection = false;

        coffee::graphics::BufferPtr pickingOutput {};
        coffee::graphics::RenderPassPtr renderPass {};
        coffee::graphics::DescriptorLayoutPtr layout {};
        PerFlightFrame<coffee::graphics::DescriptorSetPtr> descriptors {};
        coffee::graphics::GraphicsPipelinePtr pipeline {};
        coffee::graphics::FencePtr completionFence {};

        coffee::graphics::ImagePtr depthImage {};
        coffee::graphics::ImageViewPtr depthImageView {};
        coffee::graphics::FramebufferPtr framebuffer {};
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
