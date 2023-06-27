#ifndef YORNA_EDITOR_IMGUI_EXTENSIONS
#define YORNA_EDITOR_IMGUI_EXTENSIONS

#include "imgui_internal.h"

#include <coffee/utils/utils.hpp>

namespace ImGui {

    namespace Detail {

        struct InputTextCallbackUserData {
            std::string* strPtr = nullptr;
            ImGuiInputTextCallback nextCallback = nullptr;
            void* nextCallbackData = nullptr;
        };

        constexpr float DegreesToRadians(float degrees) { return degrees * (IM_PI / 180); }

        static int InputTextCallback(ImGuiInputTextCallbackData* data)
        {
            InputTextCallbackUserData* userData = static_cast<InputTextCallbackUserData*>(data->UserData);

            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                userData->strPtr->resize(data->BufTextLen);
                data->Buf = userData->strPtr->data();
            }
            else if (userData->nextCallback) {
                data->UserData = userData->nextCallbackData;
                return userData->nextCallback(data);
            }

            return 0;
        }

    } // namespace Detail

    bool DrawButtonBillboard(
        const char* text,
        const ImVec4& textColor,
        const char* imageID,
        ImTextureID textureID,
        const ImVec2& size,
        const ImVec2& textOffset = {},
        ImGuiButtonFlags flags = 0
    )
    {
        ImGuiWindow* window = GetCurrentWindow();
        ImVec2 currentCursorPosition = GetCursorScreenPos();
        ImVec2 textCursorPosition = { currentCursorPosition.x + textOffset.x, currentCursorPosition.y + textOffset.y };

        bool pressed = ImageButton(imageID, textureID, size, flags);
        window->DrawList->AddText(textCursorPosition, IM_COL32(textColor.x, textColor.y, textColor.z, textColor.w), text);

        return pressed;
    }

    bool TreeNode(const void* ptrID, const char* beginText, const char* endText, ImGuiTreeNodeFlags flags = 0)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) {
            return false;
        }

        return TreeNodeBehavior(window->GetID(ptrID), flags, beginText, endText);
    }

    bool InputText(
        const char* label,
        std::string* strPtr,
        ImGuiInputTextFlags flags = 0,
        ImGuiInputTextCallback callback = nullptr,
        void* callbackData = nullptr
    )
    {
        flags |= ImGuiInputTextFlags_CallbackResize;

        Detail::InputTextCallbackUserData inputTextCallback {};
        inputTextCallback.strPtr = strPtr;
        inputTextCallback.nextCallback = callback;
        inputTextCallback.nextCallbackData = callbackData;

        return InputText(label, strPtr->data(), strPtr->capacity() + 1, flags, Detail::InputTextCallback, &inputTextCallback);
    }

    // Exact size of icon's MUST 96 x 75 pixels or less

    inline void DrawSelectableFile(
        ImDrawList* drawList,
        const ImVec2& position,
        const std::u8string& text,
        bool& selected,
        ImGuiID id,
        ImU32 color = IM_COL32(255, 255, 255, 255)
    )
    {
        const char* textBeginPtr = reinterpret_cast<const char*>(text.data());
        const char* textEndPtr = reinterpret_cast<const char*>(text.data() + text.size());

        const float textWrapSize = 96.0f;
        ImVec2 objectNameSize = CalcTextSize(textBeginPtr, textEndPtr, false, textWrapSize);

        PushStyleVar(ImGuiStyleVar_Alpha, 0.2f);
        PushID(id);
        Selectable("", &selected, 0, { 96.0f, 75.0f + objectNameSize.y });
        PopID();
        PopStyleVar();

        drawList->AddText(nullptr, 0.0f, { position.x, position.y + 75.0f }, color, textBeginPtr, textEndPtr, textWrapSize);
    }

    void DrawFolderIcon(ImDrawList* drawList, const ImVec2& position, ImU32 color = IM_COL32(255, 255, 255, 255))
    {
        const ImVec2 folderUpperLeft = { position.x + 3.0f, position.y + 3.0f };
        const ImVec2 upperLeft = { folderUpperLeft.x, folderUpperLeft.y + 9.0f };
        const ImVec2 lowerRight = { folderUpperLeft.x + 90.0f, position.y + 72.0f };

        drawList->PathLineTo(upperLeft);
        drawList->PathLineTo({ lowerRight.x, upperLeft.y });
        drawList->PathLineTo(lowerRight);
        drawList->PathLineTo({ upperLeft.x, lowerRight.y });

        drawList->PathLineTo({ upperLeft.x, folderUpperLeft.y });
        drawList->PathLineTo({ upperLeft.x + 28.0f, folderUpperLeft.y });
        drawList->PathLineTo({ upperLeft.x + 39.0f, upperLeft.y });
        drawList->PathStroke(color, 0, 2.0f);
    }

    void DrawFileIcon(ImDrawList* drawList, const ImVec2& position, ImU32 color = IM_COL32(255, 255, 255, 255))
    {
        const ImVec2 upperLeft = { position.x + 23.0f, position.y + 3.0f };
        const ImVec2 lowerLeft = { upperLeft.x, upperLeft.y + 69.0f };
        const ImVec2 lowerRight = { position.x + 73.0f, position.y + 72.0f };

        const ImVec2 triangleFirst = { upperLeft.x + 33.0f, upperLeft.y };
        const ImVec2 triangleSecond = { triangleFirst.x, triangleFirst.y + 17.0f };
        const ImVec2 triangleThird = { triangleSecond.x + 17.0f, triangleSecond.y };

        drawList->PathLineTo(triangleFirst);
        drawList->PathLineTo(triangleSecond);
        drawList->PathLineTo(triangleThird);
        drawList->PathLineTo(lowerRight);
        drawList->PathLineTo(lowerLeft);
        drawList->PathLineTo(upperLeft);
        drawList->PathLineTo(triangleFirst);
        drawList->PathLineTo(triangleThird);
        drawList->PathStroke(color, 0, 2.0f);
    }

    void DrawUnknownFileIcon(ImDrawList* drawList, const ImVec2& position, ImU32 color = IM_COL32(255, 255, 255, 255))
    {
        DrawFileIcon(drawList, position, color);

        const ImVec2 center = { position.x + 40.0f, position.y + 44.0f };

        drawList->PathArcTo(center, 5.0f, Detail::DegreesToRadians(-180.0f), Detail::DegreesToRadians(90.0f), 16);
        drawList->PathLineTo({ center.x, center.y + 12.0f });
        drawList->PathStroke(color, 0, 2.0f);

        drawList->AddCircleFilled({ center.x, center.y + 18.0f }, 2.0f, color, 16);
    }

    void DrawMeshFileIcon(ImDrawList* drawList, const ImVec2& position, ImU32 color = IM_COL32(255, 255, 255, 255))
    {
        DrawFileIcon(drawList, position, color);

        constexpr float verticalDown = 12.0f;
        constexpr float verticalUp = 6.0f;
        constexpr float distanceBetweenFaces = (verticalDown + verticalUp) * 10.0f / 12.0f;

        const ImVec2 cubeCenter = { position.x + 44.0f, position.y + 48.0f };
        const ImVec2 lowerFace = { cubeCenter.x, cubeCenter.y + verticalDown + verticalUp };
        const ImVec2 upperFace = { cubeCenter.x, cubeCenter.y - verticalUp - verticalUp };
        const ImVec2 lowerRightFace = { cubeCenter.x + distanceBetweenFaces, cubeCenter.y + verticalDown };
        const ImVec2 upperRightFace = { cubeCenter.x + distanceBetweenFaces, cubeCenter.y - verticalUp };
        const ImVec2 lowerLeftFace = { cubeCenter.x - distanceBetweenFaces, cubeCenter.y + verticalDown };
        const ImVec2 upperLeftFace = { cubeCenter.x - distanceBetweenFaces, cubeCenter.y - verticalUp };

        drawList->PathLineTo(cubeCenter);
        drawList->PathLineTo(lowerFace);
        drawList->PathLineTo(lowerRightFace);
        drawList->PathLineTo(upperRightFace);
        drawList->PathStroke(color, 0, 2.0f);

        drawList->PathLineTo(lowerFace);
        drawList->PathLineTo(lowerLeftFace);
        drawList->PathLineTo(upperLeftFace);
        drawList->PathLineTo(upperFace);
        drawList->PathLineTo({ upperRightFace.x - 1.5f, upperRightFace.y }); // Fix for ImDrawListFlags_AntiAliasedLines
        drawList->PathLineTo(cubeCenter);
        drawList->PathLineTo(upperLeftFace);
        drawList->PathStroke(color, 0, 2.0f);
    }

    void DrawTextureFileIcon(ImDrawList* drawList, const ImVec2& position, ImU32 color = IM_COL32(255, 255, 255, 255))
    {
        DrawFileIcon(drawList, position, color);

        const ImVec2 upperLeft = { position.x + 28.0f, position.y + 40.0f };
        const ImVec2 lowerRight = { position.x + 60.0f, position.y + 62.0f };

        drawList->PathLineTo(upperLeft);
        drawList->PathLineTo({ lowerRight.x, upperLeft.y });
        drawList->PathLineTo(lowerRight);
        drawList->PathLineTo({ upperLeft.x, lowerRight.y });
        drawList->PathLineTo(upperLeft);
        drawList->PathStroke(color, 0, 2.0f);

        // TODO: Add picture
    }

    void DrawMaterialFileIcon(ImDrawList* drawList, const ImVec2& position, ImU32 color = IM_COL32(255, 255, 255, 255))
    {
        DrawFileIcon(drawList, position, color);

        const ImVec2 upperLeft = { position.x + 28.0f, position.y + 34.0f };
        const ImVec2 lowerRight = { position.x + 50.0f, position.y + 50.0f };

        // Brush

        drawList->PathLineTo(upperLeft);
        drawList->PathLineTo({ lowerRight.x, upperLeft.y });
        drawList->PathLineTo(lowerRight);
        drawList->PathLineTo({ upperLeft.x, lowerRight.y });
        drawList->PathLineTo(upperLeft);
        drawList->PathStroke(color, 0, 2.0f);

        // Dye

        const ImVec2 fp2 = { upperLeft.x + 0.0f, upperLeft.y + 12.0f };
        const ImVec2 fp3 = { upperLeft.x + 6.0f, upperLeft.y + 12.0f };
        const ImVec2 fp4 = { upperLeft.x + 6.0f, upperLeft.y + 8.0f };
        drawList->PathLineTo({ upperLeft.x + 0.0f, upperLeft.y + 8.0f });
        drawList->PathBezierCubicCurveTo(fp2, fp3, fp4, 16);

        const ImVec2 sp2 = { upperLeft.x + 6.0f, upperLeft.y + 4.0f };
        const ImVec2 sp3 = { upperLeft.x + 12.0f, upperLeft.y + 4.0f };
        const ImVec2 sp4 = { upperLeft.x + 12.0f, upperLeft.y + 8.0f };
        drawList->PathBezierCubicCurveTo(sp2, sp3, sp4, 16);
        drawList->PathLineTo({ upperLeft.x + 12.0f, upperLeft.y + 10.0f });

        const ImVec2 tp2 = { upperLeft.x + 12.0f, upperLeft.y + 14.0f };
        const ImVec2 tp3 = { upperLeft.x + 17.0f, upperLeft.y + 14.0f };
        const ImVec2 tp4 = { upperLeft.x + 17.0f, upperLeft.y + 10.0f };
        drawList->PathBezierCubicCurveTo(tp2, tp3, tp4, 16);
        drawList->PathLineTo({ upperLeft.x + 17.0f, upperLeft.y });

        drawList->PathStroke(color, 0, 2.0f);

        // Handle

        drawList->PathLineTo(lowerRight);
        drawList->PathLineTo({ lowerRight.x, lowerRight.y + 4.0f });
        drawList->PathLineTo({ lowerRight.x - 9.0f, lowerRight.y + 4.0f });
        drawList->PathLineTo({ lowerRight.x - 9.0f, lowerRight.y + 12.0f });
        drawList->PathStroke(color, 0, 2.0f);

        drawList->AddCircle({ lowerRight.x - 11.0f, lowerRight.y + 14.0f }, 3.0f, color, 16, 2.0f);

        drawList->PathLineTo({ lowerRight.x - 13.0f, lowerRight.y + 12.0f });
        drawList->PathLineTo({ lowerRight.x - 13.0f, lowerRight.y + 4.0f });
        drawList->PathLineTo({ upperLeft.x, lowerRight.y + 4.0f });
        drawList->PathLineTo({ upperLeft.x, lowerRight.y });
        drawList->PathStroke(color, 0, 2.0f);
    }

} // namespace ImGui

#endif
