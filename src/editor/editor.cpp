#include <editor/editor.hpp>

#include <editor/extensions.hpp>
#include <editor/imgui.hpp>
#include <editor/localization.hpp>

#include <entities/light_objects.hpp>

#include <json.hpp>

namespace yorna {

    constexpr float bytesToMegabytes = 1000000.0f;

    Editor::Editor(const coffee::graphics::DevicePtr& device, yorna::Yorna& gameHandle) noexcept : device { device }, gameHandle { gameHandle }
    {
        framebufferImage = gameHandle.outputSet;

        projectInformation.dialog.SetLocales(LC_ALL, "ru_RU.UTF-8", "C");
    }

    void Editor::render()
    {
        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);

        ImGui::NewFrame();

        // Allow free movement of sub-windows inside main viewport
        ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        {
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu(localizeName(LocaleName::File), !projectInformation.dialog.IsOpened())) {
                    if (ImGui::MenuItem(localizeName(LocaleName::NewProject))) {
                        projectInformation.dialog.OpenDialog("CreateProjectDialog", localizeName(LocaleName::SelectDirectory), nullptr, ".");
                        projectInformation.loaded = true;
                    }

                    if (ImGui::MenuItem(localizeName(LocaleName::OpenProject))) {
                        projectInformation.dialog.OpenDialog("LoadProjectDialog", localizeName(LocaleName::SelectCFPJFile), ".cfpj", ".");
                    }

                    if (ImGui::MenuItem(localizeName(LocaleName::SaveProject))) {
                        /* TODO */
                    }

                    if (ImGui::MenuItem(localizeName(LocaleName::CloseProject))) {
                        /* TODO actual project closing */

                        sceneHierarchy.active = false;
                        sceneViewport.active = false;
                        assetBrowser.active = false;

                        projectInformation.loaded = false;
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu(localizeName(LocaleName::Edit))) {
                    if (ImGui::MenuItem("Undo", "CTRL+Z")) {
                    }
                    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
                    } // Disabled item

                    ImGui::Separator();

                    if (ImGui::MenuItem("Cut", "CTRL+X")) {
                    }
                    if (ImGui::MenuItem("Copy", "CTRL+C")) {
                    }
                    if (ImGui::MenuItem("Paste", "CTRL+V")) {
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu(localizeName(LocaleName::Views))) {
                    ImGui::MenuItem(localizeName(LocaleName::SceneHierarchy), nullptr, &sceneHierarchy.active, projectInformation.loaded);
                    ImGui::MenuItem(localizeName(LocaleName::SceneViewport), nullptr, &sceneViewport.active, projectInformation.loaded);
                    ImGui::MenuItem(localizeName(LocaleName::AssetBrowser), nullptr, &assetBrowser.active, projectInformation.loaded);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu(localizeName(LocaleName::Language))) {
                    if (ImGui::MenuItem(reinterpret_cast<const char*>(u8"English"), nullptr, currentLocale == CurrentLocale::EN)) {
                        currentLocale = CurrentLocale::EN;
                        IGFDLocale::currentLocale = CurrentLocale::EN;
                    }

                    if (ImGui::MenuItem(reinterpret_cast<const char*>(u8"Русский"), nullptr, currentLocale == CurrentLocale::RU)) {
                        currentLocale = CurrentLocale::RU;
                        IGFDLocale::currentLocale = CurrentLocale::RU;
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            constexpr ImGuiWindowFlags fileDialogFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

            if (projectInformation.dialog.Display("LoadProjectDialog", fileDialogFlags, { 800.0f, 450.0f })) {
                if (projectInformation.dialog.IsOk()) {
                    loadProject();
                }

                projectInformation.dialog.Close();
            }

            if (sceneHierarchy.active) {
                std::string sceneHierarchyName = std::format("{}###HierarchyViewport", localizeName(LocaleName::SceneHierarchy));
                ImGui::Begin(sceneHierarchyName.data(), &sceneHierarchy.active);

                if (ImGui::BeginPopupContextWindow("SceneHierarchyRightClick", ImGuiPopupFlags_MouseButtonRight)) {
                    if (ImGui::MenuItem("New Model")) {
                        const auto entity = sceneHierarchy.components.create();

                        auto& hierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(entity);
                        hierarchyComponent.parent = entt::null;
                        hierarchyComponent.tag = "Model";

                        sceneHierarchy.components.emplace<EntityRoot>(entity);

                        // TODO: sceneHierarchy.components.emplace<Model>(); ?
                    }

                    if (ImGui::MenuItem("New Point Light")) {
                        const auto entity = sceneHierarchy.components.create();

                        auto& hierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(entity);
                        hierarchyComponent.parent = entt::null;
                        hierarchyComponent.tag = "Point Light";

                        sceneHierarchy.components.emplace<EntityRoot>(entity);

                        auto& pointLight = sceneHierarchy.components.emplace<PointLight>(entity);
                        pointLight.position = gameHandle.viewerObject.translation;
                    }

                    if (ImGui::MenuItem("New Spot Light")) {
                        const auto entity = sceneHierarchy.components.create();

                        auto& hierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(entity);
                        hierarchyComponent.parent = entt::null;
                        hierarchyComponent.tag = "Spot Light";

                        sceneHierarchy.components.emplace<EntityRoot>(entity);

                        auto& spotLight = sceneHierarchy.components.emplace<SpotLight>(entity, 30.0f, gameHandle.viewerObject.rotation);
                        spotLight.position = gameHandle.viewerObject.translation;
                    }

                    ImGui::EndPopup();
                }

                for (auto entity : sceneHierarchy.components.view<EntityRoot>()) {
                    drawHierarchyTree(entity);
                }

                ImGui::End();
            }

            if (sceneViewport.active) {
                std::string sceneViewportName = std::format("{}###SceneViewport", localizeName(LocaleName::SceneViewport));
                ImGui::Begin(sceneViewportName.data(), &sceneViewport.active);

                ImGui::Checkbox(localizeName(LocaleName::KeepAspectRatio), &sceneViewport.keepAspectRatio);
                ImGui::SameLine();
                ImGui::Checkbox(localizeName(LocaleName::DisplayFrametimeFPS), &sceneViewport.outputFramerateAndFPS);

                ImGuiIO& io = ImGui::GetIO();
                ImGuiViewport* viewport = ImGui::GetWindowViewport();

                ImVec2 currentCursorPosition = ImGui::GetCursorScreenPos();
                ImVec2 contentRegion = ImGui::GetContentRegionAvail();

                gameHandle.outputAspect
                    .store(sceneViewport.keepAspectRatio ? 16.0f / 9.0f : contentRegion.x / contentRegion.y, std::memory_order_relaxed);

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

                if (ImGui::ImageButton(
                    "VP1: Take control",
                    &framebufferImage,
                    contentRegion,
                    ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_PressedOnClick
                )) {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        // TODO: ?
                    }
                    else {
                        coffee::graphics::Window* windowHandle = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;

                        backendData->fullControlWindowPtr = windowHandle;
                        windowHandle->disableCursor();
                    }
                }

                ImGui::PopStyleVar();

                if (sceneViewport.outputFramerateAndFPS) {
                    ImVec2 textPosition = { currentCursorPosition.x + 10, currentCursorPosition.y + 10 };

                    std::string framerateAndFPS = std::format("{:.3f} ms/frame ({:.1f} FPS)",
                        calculateAverage(sceneViewport.averageFrameTime), calculateAverage(sceneViewport.averageFPS));
                    ImGui::GetWindowDrawList()->AddText(
                        textPosition, IM_COL32(0, 255, 0, 255), framerateAndFPS.data(), framerateAndFPS.data() + framerateAndFPS.size());

                    textPosition.y += 15;
                    std::string imguiCPUTime = std::format("ImGUI CPU: {:.6f} ms",
                        calculateAverage(sceneViewport.averageImGuiTime));
                    ImGui::GetWindowDrawList()->AddText(
                        textPosition, IM_COL32(0, 255, 0, 255), imguiCPUTime.data(), imguiCPUTime.data() + imguiCPUTime.size());

                    textPosition.y += 15;
                    std::string depthPass = std::format("Depth pass: {:.6f} ms",
                        calculateAverage(sceneViewport.averageDepthPass));
                    ImGui::GetWindowDrawList()->AddText(
                        textPosition, IM_COL32(0, 255, 0, 255), depthPass.data(), depthPass.data() + depthPass.size());

                    textPosition.y += 15;
                    std::string rendering = std::format("Rendering: {:.6f} ms",
                        calculateAverage(sceneViewport.averageRendering));
                    ImGui::GetWindowDrawList()->AddText(
                        textPosition, IM_COL32(0, 255, 0, 255), rendering.data(), rendering.data() + rendering.size());

                    textPosition.y += 15;
                    std::array<VmaBudget, VK_MAX_MEMORY_HEAPS> budgets = device->heapBudgets();

                    if (sceneViewport.deviceHeapIndex != SceneViewport::kInvalidHeapIndex) {
                        VmaBudget& budget = budgets[sceneViewport.deviceHeapIndex];
                        float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.0f;

                        textPosition.y += 15;
                        std::string overallUsage = std::format("VRAM: {:.2f}/{:.2f} mb ({:.2f}%)",
                            budget.usage / bytesToMegabytes,
                            budget.budget / bytesToMegabytes,
                            percentage
                        );

                        ImGui::GetWindowDrawList()->AddText(
                            textPosition,
                            IM_COL32(0, 255, 0, 255),
                            overallUsage.data(),
                            overallUsage.data() + overallUsage.size()
                        );
                    }

                    if (sceneViewport.hostHeapIndex != SceneViewport::kInvalidHeapIndex) {
                        VmaBudget& budget = budgets[sceneViewport.hostHeapIndex];
                        float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.0f;

                        textPosition.y += 15;
                        std::string overallUsage = std::format(
                            "RAM: {:.2f}/{:.2f} mb ({:.2f}%)",
                            budget.usage / bytesToMegabytes,
                            budget.budget / bytesToMegabytes,
                            percentage
                        );

                        ImGui::GetWindowDrawList()->AddText(
                            textPosition,
                            IM_COL32(0, 255, 0, 255),
                            overallUsage.data(),
                            overallUsage.data() + overallUsage.size()
                        );
                    }

                    if (sceneViewport.sharedHeapIndex != SceneViewport::kInvalidHeapIndex) {
                        VmaBudget& budget = budgets[sceneViewport.sharedHeapIndex];
                        float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.0f;

                        textPosition.y += 15;
                        std::string overallUsage = std::format(
                            "Shared: {:.2f}/{:.2f} mb ({:.2f}%)",
                            budget.usage / bytesToMegabytes,
                            budget.budget / bytesToMegabytes,
                            percentage
                        );

                        ImGui::GetWindowDrawList()->AddText(
                            textPosition,
                            IM_COL32(0, 255, 0, 255),
                            overallUsage.data(),
                            overallUsage.data() + overallUsage.size()
                        );
                    }
                }

                ImGui::End();
            }

            if (assetBrowser.active) {
                std::string assetViewportName = std::format("{}###AssetBrowser", localizeName(LocaleName::AssetBrowser));
                ImGui::Begin(assetViewportName.data(), &assetBrowser.active);

                if (assetBrowser.updateRequired) {
                    loadAssetDirectory();
                }

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 originalContentRegion = ImGui::GetContentRegionAvail();
                ImVec2 remainingContentRegion = originalContentRegion;

                for (size_t i = 0; i < assetBrowser.content.size() && !assetBrowser.updateRequired; i++) {
                    DirectoryObject& directoryObject = assetBrowser.content[i];
                    ImVec2 cursorPosition = ImGui::GetCursorScreenPos();

                    ImGui::DrawSelectableFile(drawList, cursorPosition, directoryObject.filename, directoryObject.selected, i);

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        switch (directoryObject.type) {
                            case DirectoryObject::Type::Directory:
                                assetBrowser.updateRequired = true;
                                assetBrowser.currentFolder = assetBrowser.rootFolder / directoryObject.relativePath;
                                break;
                        }
                    }

                    switch (directoryObject.type) {
                        default:
                        case DirectoryObject::Type::Unknown:
                            ImGui::DrawUnknownFileIcon(drawList, cursorPosition);
                            break;
                        case DirectoryObject::Type::Directory:
                            ImGui::DrawFolderIcon(drawList, cursorPosition);
                            break;
                        case DirectoryObject::Type::Mesh:
                            ImGui::DrawMeshFileIcon(drawList, cursorPosition);
                            break;
                        case DirectoryObject::Type::Texture:
                            ImGui::DrawTextureFileIcon(drawList, cursorPosition);
                            break;
                    }

                    if ((remainingContentRegion.x -= (96.0f + ImGui::GetStyle().ItemSpacing.x)) < 96.0f) {
                        ImGui::NewLine();
                        remainingContentRegion = originalContentRegion;
                        continue;
                    }

                    ImGui::SameLine();
                }

                ImGui::End();
            }
        }

        ImGui::EndFrame();

        ImGui::Render();
    }

    void Editor::updateAverageTimings(float deltaTime, float imguiTiming, QueryTimestamps& queryTimestamps)
    {
        sceneViewport.averageFPS[sceneViewport.statisticIndex] = 1.0f / deltaTime;
        sceneViewport.averageFrameTime[sceneViewport.statisticIndex] = 1000.0f * deltaTime;
        sceneViewport.averageImGuiTime[sceneViewport.statisticIndex] = imguiTiming;

        std::vector<float> gpuTimings = queryTimestamps.extractResults();

        if (gpuTimings.back() > 0) {
            sceneViewport.averageDepthPass[sceneViewport.statisticIndex] = gpuTimings[0];
            sceneViewport.averageRendering[sceneViewport.statisticIndex] = gpuTimings[1];
        }

        sceneViewport.statisticIndex = (sceneViewport.statisticIndex + 1) % SceneViewport::kAverageStatisticBufferSize;
    }

    void Editor::createProject()
    {

    }

    void Editor::loadProject()
    {

    }

    void Editor::loadAssetDirectory()
    {
        assetBrowser.content.clear();

        // This system required here as coffee::Filesystem doesn't support directories as entries (because it's doesn't required by design)
        for (std::filesystem::directory_entry directoryObject : std::filesystem::directory_iterator { assetBrowser.currentFolder }) {
            auto& newEntry = assetBrowser.content.emplace_back();

            std::filesystem::path path = directoryObject.path();
            newEntry.relativePath = std::filesystem::relative(path, assetBrowser.rootFolder);
            newEntry.filename = path.filename().u8string();

            if (directoryObject.is_directory()) {
                newEntry.type = DirectoryObject::Type::Directory;
            }
            else if (directoryObject.is_regular_file()) {
                std::string extension = path.extension().string();

                switch (coffee::utils::fnv1a::digest(extension.data())) {
                    case coffee::utils::fnv1a::digest(".basis"):
                    case coffee::utils::fnv1a::digest(".ktx2"):
                        newEntry.type = DirectoryObject::Type::Texture;
                        break;
                    case coffee::utils::fnv1a::digest(".cfa"):
                        newEntry.type = DirectoryObject::Type::Mesh;
                        break;
                    case coffee::utils::fnv1a::digest(".vert"):
                    case coffee::utils::fnv1a::digest(".frag"):
                    case coffee::utils::fnv1a::digest(".glsl"):
                        newEntry.type = DirectoryObject::Type::Shader;
                        break;
                    case coffee::utils::fnv1a::digest(".spv"):
                        newEntry.type = DirectoryObject::Type::CompiledShader;
                        break;
                    default:
                        break;
                }
            }
        }

        assetBrowser.updateRequired = false;
    }

    void Editor::saveProject()
    {
        std::string rawFilePath = projectInformation.dialog.GetFilePathName();
        std::filesystem::path filePath = std::u8string{ rawFilePath.begin(), rawFilePath.end() };

        std::ifstream stream{ filePath };
        nlohmann::json parsedProject = nlohmann::json::parse(stream);

        constexpr uint32_t currentVersion = 1;
        uint32_t projectVersion = parsedProject["version"].get<uint32_t>();

        if (projectVersion > currentVersion) {
            COFFEE_WARNING("Loaded project was created from newer version of editor. Some features might not work or even crash.");
        }

        if (projectVersion < currentVersion) {
            COFFEE_WARNING("Loaded project was created from older version of editor. Some features might be deprecated and removed.");
        }

        assetBrowser.rootFolder = std::filesystem::path(rawFilePath).parent_path() / parsedProject["assetsFolder"].get<std::string>();
        assetBrowser.currentFolder = assetBrowser.rootFolder;

        if (!std::filesystem::exists(assetBrowser.rootFolder)) {
            COFFEE_WARNING("Loaded project file is probably damaged: assetsFolder is not exist.");
            COFFEE_WARNING("assetsFolder was created for you in {}", std::filesystem::canonical(assetBrowser.rootFolder).string());

            std::filesystem::create_directories(assetBrowser.rootFolder);
        }

        loadAssetDirectory();

        // TODO: Scene loading

        projectInformation.loaded = true;
    }

    void Editor::removeChilds(entt::entity childEntity)
    {
        while (childEntity != entt::null) {
            auto& childComponent = sceneHierarchy.components.get<HierarchyComponent>(childEntity);

            removeChilds(childComponent.firstChild);
            childComponent.firstChild = entt::null;

            entt::entity nextChild = childComponent.next;
            sceneHierarchy.components.destroy(childEntity);

            childEntity = nextChild;
        }
    }

    void Editor::drawHierarchyTree(entt::entity entity)
    {
        while (entity != entt::null) {
            auto& hierachyComponent = sceneHierarchy.components.get<HierarchyComponent>(entity);
            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

            if (hierachyComponent.firstChild == entt::null) {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            bool opened = ImGui::TreeNode(
                reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<std::underlying_type_t<entt::entity>>(entity))),
                hierachyComponent.tag.data(), hierachyComponent.tag.data() + hierachyComponent.tag.size(), flags
            );

            if (ImGui::BeginPopupContextItem(nullptr, ImGuiMouseButton_Right)) {
                if (ImGui::MenuItem("New Model")) {
                    const auto child = sceneHierarchy.components.create();

                    entt::entity previousChild = hierachyComponent.firstChild;
                    hierachyComponent.firstChild = child;

                    auto& childHierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(child);
                    childHierarchyComponent.parent = entity;
                    childHierarchyComponent.next = previousChild;
                    childHierarchyComponent.tag = "Model";

                    if (auto* previousChildHierarchyComponent = sceneHierarchy.components.try_get<HierarchyComponent>(previousChild)) {
                        previousChildHierarchyComponent->previous = child;
                    }

                    // TODO: sceneHierarchy.components.emplace<Model>(); ?
                }

                if (ImGui::MenuItem("New Point Light")) {
                    const auto child = sceneHierarchy.components.create();

                    entt::entity previousChild = hierachyComponent.firstChild;
                    hierachyComponent.firstChild = child;

                    auto& childHierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(child);
                    childHierarchyComponent.parent = entity;
                    childHierarchyComponent.next = previousChild;
                    childHierarchyComponent.tag = "Point Light";

                    if (auto* previousChildHierarchyComponent = sceneHierarchy.components.try_get<HierarchyComponent>(previousChild)) {
                        previousChildHierarchyComponent->previous = child;
                    }

                    auto& pointLight = sceneHierarchy.components.emplace<PointLight>(entity);
                    pointLight.position = gameHandle.viewerObject.translation;
                }

                if (ImGui::MenuItem("New Spot Light")) {
                    const auto child = sceneHierarchy.components.create();

                    entt::entity previousChild = hierachyComponent.firstChild;
                    hierachyComponent.firstChild = child;

                    auto& childHierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(child);
                    childHierarchyComponent.parent = entity;
                    childHierarchyComponent.next = previousChild;
                    childHierarchyComponent.tag = "Spot Light";

                    if (auto* previousChildHierarchyComponent = sceneHierarchy.components.try_get<HierarchyComponent>(previousChild)) {
                        previousChildHierarchyComponent->previous = child;
                    }

                    auto& spotLight = sceneHierarchy.components.emplace<SpotLight>(entity, 30.0f, gameHandle.viewerObject.rotation);
                    spotLight.position = gameHandle.viewerObject.translation;
                }

                if (ImGui::MenuItem("Rename")) {
                    sceneHierarchy.renameEntity = entity;
                }

                if (ImGui::MenuItem("Delete")) {
                    sceneHierarchy.deleteEntity = entity;
                }

                ImGui::EndPopup();
            }

            if (sceneHierarchy.renameEntity == entity) {
                ImGui::OpenPopup("Enter new name");

                ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, { 0.5f, 0.5f });

                ImGui::BeginPopupModal("Enter new name", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::InputText("Entity Name", &hierachyComponent.tag);

                if (ImGui::Button("OK", { 64.0f, 0.0f })) {
                    if (hierachyComponent.tag.find_first_not_of(" \t\n\v\f\r") == std::string::npos) {
                        hierachyComponent.tag = "Object"; // Return to fallback value so entity won't left invisible
                    }

                    ImGui::CloseCurrentPopup();
                    sceneHierarchy.renameEntity = entt::null;
                }

                ImGui::EndPopup();
            }

            if (sceneHierarchy.deleteEntity == entity) {
                ImGui::OpenPopup("Are you sure?");

                ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, { 0.5f, 0.5f });

                ImGui::BeginPopupModal("Are you sure?", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Are you sure that you wanna delete entity '%s' and WHOLE SUBTREE?", hierachyComponent.tag.data());
                ImGui::TextUnformatted("This action cannot be undone!");

                if (ImGui::Button("Yes", { 64.0f, 0.0f })) {
                    if (hierachyComponent.previous != entt::null) {
                        auto& previousHierarchyComponent = sceneHierarchy.components.get<HierarchyComponent>(hierachyComponent.previous);

                        if (hierachyComponent.next != entt::null) {
                            sceneHierarchy.components.get<HierarchyComponent>(hierachyComponent.next).previous = hierachyComponent.previous;
                        }

                        previousHierarchyComponent.next = entt::null;
                    }
                    else {
                        if (hierachyComponent.parent != entt::null) {
                            sceneHierarchy.components.get<HierarchyComponent>(hierachyComponent.parent).firstChild = hierachyComponent.next;
                        }
                    }

                    removeChilds(hierachyComponent.firstChild);
                    hierachyComponent.firstChild = entt::null;
                    sceneHierarchy.components.destroy(entity);

                    ImGui::CloseCurrentPopup();
                    sceneHierarchy.deleteEntity = entt::null;
                }

                ImGui::SameLine();

                if (ImGui::Button("No", { 64.0f, 0.0f })) {
                    ImGui::CloseCurrentPopup();
                    sceneHierarchy.deleteEntity = entt::null;
                }

                ImGui::EndPopup();
            }

            if (opened) {
                if (hierachyComponent.firstChild != entt::null) {
                    drawHierarchyTree(hierachyComponent.firstChild);
                }

                ImGui::TreePop();
            }

            entity = hierachyComponent.next;
        }
    }

}