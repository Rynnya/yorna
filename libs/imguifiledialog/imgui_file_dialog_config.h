#pragma once

// uncomment and modify defines under for customize ImGuiFileDialog

#define IMGUIFILEDIALOG_NO_EXPORT

//#define MAX_FILE_DIALOG_NAME_BUFFER 1024
//#define MAX_PATH_BUFFER_SIZE 1024

// the slash's buttons in path cna be used for quick select parallles directories
//#define USE_QUICK_PATH_SELECT

// the spacing between button path's can be customized. 
// if disabled the spacing is defined by the imgui theme
// define the space between path buttons 
//#define CUSTOM_PATH_SPACING 2

//#define USE_EXPLORATION_BY_KEYS
// this mapping by default is for GLFW but you can use another
//#include <GLFW/glfw3.h> 
// Up key for explore to the top
//#define IGFD_KEY_UP ImGuiKey_UpArrow
// Down key for explore to the bottom
//#define IGFD_KEY_DOWN ImGuiKey_DownArrow
// Enter key for open directory
//#define IGFD_KEY_ENTER ImGuiKey_Enter
// BackSpace for comming back to the last directory
//#define IGFD_KEY_BACKSPACE ImGuiKey_Backspace

// by ex you can quit the dialog by pressing the key excape
//#define USE_DIALOG_EXIT_WITH_KEY
//#define IGFD_EXIT_KEY ImGuiKey_Escape

// widget
// begin combo widget
// #define IMGUI_BEGIN_COMBO ImGui::BeginCombo
// filter combobox width
//#define FILTER_COMBO_WIDTH 120.0f
// button widget use for compose path
//#define IMGUI_PATH_BUTTON ImGui::Button
// standard button
//#define IMGUI_BUTTON ImGui::Button

#include <array>
#include <cstddef>

#include "imgui_file_dialog_def.h"

namespace IGFDLocale {

    constexpr size_t localeSize = 23;
    inline CurrentLocale currentLocale = CurrentLocale::EN;

    static inline std::array<const char8_t*, localeSize> enLocale = {
        u8"Drives",
        u8"Search",
        u8"[DIR] ",
        u8"[LINK] ",
        u8"[FILE] ",
        u8"File Name: ",
        u8"Directory Path: ",
        u8"Reset Search",
        u8"Edit path\nYou can also right click on path buttons",
        u8"Reset to current directory",
        u8"Create Directory",
        u8"This file already exist!",
        u8"Would you like to overwrite it?",
        u8"Confirm",
        u8"Cancel",
        u8"File name",
        u8"Type",
        u8"Size",
        u8"Date",
        u8"B",
        u8"KB",
        u8"MB",
        u8"GB",
    };

    static inline std::array<const char8_t*, localeSize> ruLocale = {
        u8"Диски",
        u8"Поиск",
        u8"[Папка] ",
        u8"[Ссылка] ",
        u8"[Файл] ",
        u8"Имя файла: ",
        u8"Путь к папке: ",
        u8"Сбросить поиск",
        u8"Изменить путь\nВы также можете нажать ПКМ по кнопкам пути",
        u8"Вернуться к начальной папке",
        u8"Создать папку",
        u8"Этот файл уже существует!",
        u8"Хотите его перезаписать?",
        u8"Подтвердить",
        u8"Отменить",
        u8"Имя файла",
        u8"Тип",
        u8"Размер",
        u8"Дата",
        u8"б",
        u8"Кб",
        u8"Мб",
        u8"Гб",
    };

    enum class Name : uint32_t {
        Drives,
        Search,
        DIR,
        LINK,
        FILE,
        FileName,
        DirectoryPath,
        ResetSearch,
        EditPath,
        ResetDirectory,
        CreateFolder,
        FileAlreadyExist,
        OverwriteIt,
        Confirm,
        Cancel,
        ToolbarFileName,
        ToolbarType,
        ToolbarSize,
        ToolbarDate,
        Bytes,
        KiloBytes,
        MegaBytes,
        GigaBytes,
    };

    static inline const char* localizeName(Name localeName) noexcept {
        switch (currentLocale) {
            case CurrentLocale::RU:
                return reinterpret_cast<const char*>(ruLocale[static_cast<uint32_t>(localeName)]);
            default:
                return reinterpret_cast<const char*>(enLocale[static_cast<uint32_t>(localeName)]);
        }
    }
}

// locales string
//#define createDirButtonString "+"
//#define resetButtonString "R"
#define drivesButtonString IGFDLocale::localizeName(IGFDLocale::Name::Drives)
//#define editPathButtonString "E"
#define searchString IGFDLocale::localizeName(IGFDLocale::Name::Search)
#define dirEntryString IGFDLocale::localizeName(IGFDLocale::Name::DIR)
#define linkEntryString IGFDLocale::localizeName(IGFDLocale::Name::LINK)
#define fileEntryString IGFDLocale::localizeName(IGFDLocale::Name::FILE)
#define fileNameString IGFDLocale::localizeName(IGFDLocale::Name::FileName)
#define dirNameString IGFDLocale::localizeName(IGFDLocale::Name::DirectoryPath)
#define buttonResetSearchString IGFDLocale::localizeName(IGFDLocale::Name::ResetSearch)
#define buttonDriveString IGFDLocale::localizeName(IGFDLocale::Name::Drives)
#define buttonEditPathString IGFDLocale::localizeName(IGFDLocale::Name::EditPath)
#define buttonResetPathString IGFDLocale::localizeName(IGFDLocale::Name::ResetDirectory)
#define buttonCreateDirString IGFDLocale::localizeName(IGFDLocale::Name::CreateFolder)
#define OverWriteDialogTitleString IGFDLocale::localizeName(IGFDLocale::Name::FileAlreadyExist)
#define OverWriteDialogMessageString IGFDLocale::localizeName(IGFDLocale::Name::OverwriteIt)
#define OverWriteDialogConfirmButtonString IGFDLocale::localizeName(IGFDLocale::Name::Confirm)
#define OverWriteDialogCancelButtonString IGFDLocale::localizeName(IGFDLocale::Name::Cancel)

//Validation buttons
//#define okButtonString " OK"
//#define okButtonWidth 0.0f
#define cancelButtonString IGFDLocale::localizeName(IGFDLocale::Name::Cancel)
//#define cancelButtonWidth 0.0f
//alignement [0:1], 0.0 is left, 0.5 middle, 1.0 right, and other ratios
//#define okCancelButtonAlignement 0.0f
//#define invertOkAndCancelButtons 0

// DateTimeFormat
// see strftime functionin <ctime> for customize
// "%Y/%m/%d %H:%M" give 2021/01/22 11:47
// "%Y/%m/%d %i:%M%p" give 2021/01/22 11:45PM
//#define DateTimeFormat "%Y/%m/%d %i:%M%p"

// theses icons will appear in table headers
//#define USE_CUSTOM_SORTING_ICON
//#define tableHeaderAscendingIcon "A|"
//#define tableHeaderDescendingIcon "D|"
#define tableHeaderFileNameString IGFDLocale::localizeName(IGFDLocale::Name::ToolbarFileName)
#define tableHeaderFileTypeString IGFDLocale::localizeName(IGFDLocale::Name::ToolbarType)
#define tableHeaderFileSizeString IGFDLocale::localizeName(IGFDLocale::Name::ToolbarSize)
#define tableHeaderFileDateString IGFDLocale::localizeName(IGFDLocale::Name::ToolbarDate)
#define fileSizeBytes IGFDLocale::localizeName(IGFDLocale::Name::Bytes)
#define fileSizeKiloBytes IGFDLocale::localizeName(IGFDLocale::Name::KiloBytes)
#define fileSizeMegaBytes IGFDLocale::localizeName(IGFDLocale::Name::MegaBytes)
#define fileSizeGigaBytes IGFDLocale::localizeName(IGFDLocale::Name::GigaBytes)

// default table sort field (must be FIELD_FILENAME, FIELD_TYPE, FIELD_SIZE, FIELD_DATE or FIELD_THUMBNAILS)
//#define defaultSortField FIELD_FILENAME

// default table sort order for each field (true => Descending, false => Ascending)
//#define defaultSortOrderFilename true
//#define defaultSortOrderType true
//#define defaultSortOrderSize true
//#define defaultSortOrderDate true
//#define defaultSortOrderThumbnails true

//#define USE_BOOKMARK
//#define bookmarkPaneWith 150.0f
//#define IMGUI_TOGGLE_BUTTON ToggleButton
//#define bookmarksButtonString "Bookmark"
//#define bookmarksButtonHelpString "Bookmark"
//#define addBookmarkButtonString "+"
//#define removeBookmarkButtonString "-"
