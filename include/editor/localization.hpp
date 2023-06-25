#ifndef YORNA_EDITOR_LOCALIZATION
#define YORNA_EDITOR_LOCALIZATION

#include <array>

#include <imgui_file_dialog_def.h>

namespace yorna {

    namespace detail {
        constexpr size_t localeSize = 24;

        std::array<const char8_t*, localeSize> enLocale = {
            u8"File",
            u8"Edit",
            u8"Views",
            u8"Language",
            u8"New Project",
            u8"Select Directory",
            u8"Open Project",
            u8"Select .cfpj file",
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
            u8"Файл",
            u8"Правка",
            u8"Виды",
            u8"Язык",
            u8"Создать проект",
            u8"Выберите папку",
            u8"Открыть проект",
            u8"Выберите .cfpj файл",
            u8"Сохранить проект",
            u8"Закрыть проект",
            u8"Иерархия сцены",
            u8"Вид сцены",
            u8"Просмотр сцены",
            u8"Поддерживать соотношение сторон 16:9",
            u8"Показывать время кадра/FPS",
            u8"<Безымянный>",
            u8"",
        };

    } // namespace detail

    CurrentLocale currentLocale = CurrentLocale::EN;

    enum class LocaleName : uint32_t {
        File,
        Edit,
        Views,
        Language,
        NewProject,
        SelectDirectory,
        OpenProject,
        SelectCFPJFile,
        SaveProject,
        CloseProject,
        SceneHierarchy,
        SceneViewport,
        AssetBrowser,
        KeepAspectRatio,
        DisplayFrametimeFPS,
        UnnamedEntity,
    };

    inline const char* localizeName(LocaleName name) noexcept
    {
        switch (currentLocale) {
            case CurrentLocale::RU:
                return reinterpret_cast<const char*>(detail::ruLocale[static_cast<uint32_t>(name)]);
            default:
                return reinterpret_cast<const char*>(detail::enLocale[static_cast<uint32_t>(name)]);
        }
    }

} // namespace yorna

#endif