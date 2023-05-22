#ifndef SYSTEMS_IMGUI_LOCALIZATION
#define SYSTEMS_IMGUI_LOCALIZATION

#include <array>

#include <imgui_file_dialog_def.h>

namespace editor {

    namespace detail {
        constexpr size_t localeSize = 24;

        std::array<const char8_t*, localeSize> enLocale = {
            u8"File",
            u8"Edit",
            u8"Views",
            u8"Language",
            u8"New Project",
            u8"Open Project",
            u8"Save Project",
            u8"Close Project",
            u8"Scene Hierarchy",
            u8"Scene Viewport",
            u8"Asset Browser",
            u8"Keep the aspect ratio 16:9",
            u8"Display Frametime/FPS",
            u8"<Unnamed>",
            u8"",
        };

        std::array<const char8_t*, localeSize> ruLocale = {
            u8"����",
            u8"������",
            u8"����",
            u8"����",
            u8"����� ������",
            u8"������� ������",
            u8"��������� ������",
            u8"������� ������",
            u8"�������� �����",
            u8"��� �����",
            u8"������� �������",
            u8"��������� ����������� ������ 16:9",
            u8"���������� ����� �����/FPS",
            u8"<����������>",
            u8"",
        };
    }

    CurrentLocale currentLocale = CurrentLocale::EN;

    enum class LocaleName : uint32_t {
        File,
        Edit,
        Views,
        Language,
        NewProject,
        OpenProject,
        SaveProject,
        CloseProject,
        HierarchyComponent,
        SceneViewport,
        AssetBrowser,
        KeepAspectRatio,
        DisplayFrametimeFPS,
        UnnamedEntity,
    };

    inline const char* localizeName(LocaleName name) noexcept {
        switch (currentLocale) {
            case CurrentLocale::RU:
                return reinterpret_cast<const char*>(detail::ruLocale[static_cast<uint32_t>(name)]);
            default:
                return reinterpret_cast<const char*>(detail::enLocale[static_cast<uint32_t>(name)]);
        } 
    }

}

#endif