#include <editor/editor.hpp>

#include <editor/extensions.hpp>
#include <editor/imgui.hpp>
#include <editor/localization.hpp>

#include <entities/light_objects.hpp>

#include <ImGuizmo.h>
#include <json.hpp>

namespace yorna {

    constexpr float bytesToMegabytes = 1000000.0f;

    Editor::Editor(const coffee::graphics::DevicePtr& device, yorna::Yorna& gameHandle) noexcept : device { device }, gameHandle { gameHandle }
    {
        freePointLightIndices.fill(true);
        freeSpotLightIndices.fill(true);

        const VkPhysicalDeviceMemoryProperties& properties = device->memoryProperties();

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
            const VkMemoryType& memoryType = properties.memoryTypes[i];

            bool isDeviceMemory = (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0;
            bool isHostMemory = (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;

            if (isDeviceMemory && isHostMemory) {
                sceneViewport.sharedHeapIndex = memoryType.heapIndex;
            }
            else if (isDeviceMemory) {
                sceneViewport.deviceHeapIndex = memoryType.heapIndex;
            }
            else if (isHostMemory) {
                sceneViewport.hostHeapIndex = memoryType.heapIndex;
            }
        }

        projectInformation.dialog.SetLocales(LC_ALL, "ru_RU.UTF-8", "C");

        framebufferImage = gameHandle.outputDescriptor;
    }

    void Editor::render()
    {
        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);

        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        // Allow free movement of sub-windows inside main viewport
        ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

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
                ImGui::MenuItem(localizeName(LocaleName::GuizmoViewport), nullptr, &guizmoViewport.active, projectInformation.loaded);
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

        constexpr ImGuiWindowFlags kFileDialogFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

        if (projectInformation.dialog.Display("LoadProjectDialog", kFileDialogFlags, { 800.0f, 450.0f })) {
            if (projectInformation.dialog.IsOk()) {
                loadProject();
            }

            projectInformation.dialog.Close();
        }

        if (sceneHierarchy.active) {
            std::string sceneHierarchyName = std::format("{}###HierarchyViewport", localizeName(LocaleName::SceneHierarchy));
            ImGui::Begin(sceneHierarchyName.data(), &sceneHierarchy.active);

            if (ImGui::BeginPopupContextWindow("SceneHierarchyRightClick", ImGuiPopupFlags_MouseButtonRight)) {
                if (ImGui::MenuItem("New Folder")) {
                    static_cast<void>(insertNewRootNode(HierarchyType::Folder));
                }

                if (ImGui::MenuItem("New Model")) {
                    static_cast<void>(insertNewRootNode(HierarchyType::Model));

                    // TODO: sceneHierarchy.components.emplace<Model>(); ?
                }

                if (ImGui::MenuItem("New Point Light", nullptr, nullptr, pointLightCount < freePointLightIndices.size())) {
                    acquirePointLightIndex(insertNewRootNode(HierarchyType::PointLight));
                }

                if (ImGui::MenuItem("New Spot Light", nullptr, nullptr, spotLightCount < freeSpotLightIndices.size())) {
                    acquireSpotLightIndex(insertNewRootNode(HierarchyType::SpotLight));
                }

                ImGui::EndPopup();
            }

            for (auto entity : sceneHierarchy.components.view<EntityRoot>()) {
                drawHierarchyTree(entity);
            }

            ImGui::End();
        }

        if (guizmoViewport.active) {
            std::string guizmoViewportName = std::format("{}###GuizmoViewport", localizeName(LocaleName::GuizmoViewport));
            ImGui::Begin(guizmoViewportName.data(), &guizmoViewport.active);

            if (ImGui::RadioButton("Translate", guizmoViewport.operation == ImGuizmo::TRANSLATE)) {
                guizmoViewport.operation = ImGuizmo::TRANSLATE;
            }
            ImGui::SameLine();

            if (ImGui::RadioButton("Rotate", guizmoViewport.operation == ImGuizmo::ROTATE)) {
                guizmoViewport.operation = ImGuizmo::ROTATE;
            }
            ImGui::SameLine();

            if (ImGui::RadioButton("Scale", guizmoViewport.operation == ImGuizmo::SCALE)) {
                guizmoViewport.operation = ImGuizmo::SCALE;
            }

            // TODO: Add new settings

            ImGui::End();
        }

        if (sceneViewport.active) {
            std::string sceneViewportName = std::format("{}###SceneViewport", localizeName(LocaleName::SceneViewport));
            ImGui::Begin(sceneViewportName.data(), &sceneViewport.active);

            ImGui::Checkbox(localizeName(LocaleName::KeepAspectRatio), &sceneViewport.keepAspectRatio);
            ImGui::SameLine();
            ImGui::Checkbox(localizeName(LocaleName::DisplayFrametimeFPS), &sceneViewport.outputFramerateAndFPS);

            ImGuiViewport* viewport = ImGui::GetWindowViewport();
            ImVec2 currentCursorPosition = ImGui::GetCursorScreenPos();
            sceneViewport.size = ImGui::GetContentRegionAvail();

            float newAspectRatio = sceneViewport.keepAspectRatio ? kDefaultAspectRatio : sceneViewport.size.x / sceneViewport.size.y;
            gameHandle.outputAspect.store(newAspectRatio, std::memory_order_relaxed);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

            constexpr ImGuiButtonFlags kOutputImageFlags =
                ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_PressedOnClick;

            if (ImGui::ImageButton("VP1: Take control", &framebufferImage, sceneViewport.size, kOutputImageFlags)) {
                ImVec2 clickPosition = ImGui::GetIO().MousePos;

                if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    glm::vec4 viewport = { 0.0f, 0.0f, gameHandle.outputViewport };
                    glm::vec3 nearSource = { clickPosition.x, clickPosition.y, 0.0f };
                    glm::vec3 farSource = { clickPosition.x, clickPosition.y, 1.0f };
                    glm::vec3 nearPoint = glm::unProject(nearSource, gameHandle.camera.view(), gameHandle.camera.projection(), viewport);
                    glm::vec3 farPoint = glm::unProject(farSource, gameHandle.camera.view(), gameHandle.camera.projection(), viewport);

                    glm::vec3 direction = glm::normalize(farPoint - nearPoint);
                    constexpr glm::vec3 up = { 0.0f, 1.0f, 0.0f };

                    float distance = -(glm::dot(up, direction) / glm::dot(up, nearPoint));
                    glm::vec3 pickedPosition = nearPoint + direction * distance;

                    glm::vec2 www {};
                    auto x = glm::dot(www, www);
                    // TODO: Raycast from mouse to closest object and select it as a target for details menu and guizmo
                }
                else {
                    coffee::graphics::Window* windowHandle = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;

                    backendData->fullControlWindowPtr = windowHandle;
                    windowHandle->disableCursor();
                }
            }

            ImGui::PopStyleVar();

            if (guizmoViewport.affectedModel != nullptr) {
                glm::mat4 transformationMatrix = guizmoViewport.affectedModel->mat4();

                ImGuizmo::SetRect(0, 0, sceneViewport.size.x, sceneViewport.size.y);
                ImGuizmo::Manipulate(
                    reinterpret_cast<const float*>(&gameHandle.camera.view()),
                    reinterpret_cast<const float*>(&gameHandle.camera.projection()),
                    guizmoViewport.operation,
                    guizmoViewport.mode,
                    reinterpret_cast<float*>(&transformationMatrix)
                );

                ImGuizmo::DecomposeMatrixToComponents(
                    reinterpret_cast<float*>(&transformationMatrix),
                    reinterpret_cast<float*>(&guizmoViewport.affectedModel->translation),
                    reinterpret_cast<float*>(&guizmoViewport.affectedModel->rotation),
                    reinterpret_cast<float*>(&guizmoViewport.affectedModel->scale)
                );
            }

            if (sceneViewport.outputFramerateAndFPS) {
                sceneViewport.framerateTextPosition = { currentCursorPosition.x + 10, currentCursorPosition.y + 10 };

                outputFramerateInfo("{:.3f} ms/frame", sceneViewport.averageFrameTime);
                outputFramerateInfo("{:.1f} FPS", sceneViewport.averageFPS);
                outputFramerateInfo("ImGUI CPU: {:.6f} ms", sceneViewport.averageImGuiTime);
                outputFramerateInfo("Depth pass: {:.6f} ms", sceneViewport.averageDepthPass);
                outputFramerateInfo("Light pass: {:.6f} ms", sceneViewport.averageLightPass);
                outputFramerateInfo("Rendering: {:.6f} ms", sceneViewport.averageRendering);

                std::array<VmaBudget, VK_MAX_MEMORY_HEAPS> budgets = device->heapBudgets();

                outputMemoryUsage(budgets, "VRAM", sceneViewport.deviceHeapIndex);
                outputMemoryUsage(budgets, "RAM", sceneViewport.hostHeapIndex);
                outputMemoryUsage(budgets, "Shared", sceneViewport.sharedHeapIndex);
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
                bool isDirectoryObjectClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

                if (isDirectoryObjectClicked && directoryObject.type == DirectoryObject::Type::Directory) {
                    assetBrowser.updateRequired = true;
                    assetBrowser.currentFolder = assetBrowser.rootFolder / directoryObject.relativePath;
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

                constexpr float kDirectorySpacingSize = 96.0f;
                remainingContentRegion.x -= (kDirectorySpacingSize + ImGui::GetStyle().ItemSpacing.x);

                if (remainingContentRegion.x < kDirectorySpacingSize) {
                    ImGui::NewLine();
                    remainingContentRegion = originalContentRegion;

                    continue;
                }

                ImGui::SameLine();
            }

            ImGui::End();
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

        if (gpuTimings.back() > 0.0f) {
            sceneViewport.averageDepthPass[sceneViewport.statisticIndex] = gpuTimings[0];
            sceneViewport.averageLightPass[sceneViewport.statisticIndex] = gpuTimings[1];
            sceneViewport.averageRendering[sceneViewport.statisticIndex] = gpuTimings[2];
        }

        sceneViewport.statisticIndex = (sceneViewport.statisticIndex + 1) % SceneViewport::kAverageStatisticBufferSize;
    }

    void Editor::createProject() {}

    void Editor::loadProject() {}

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
        std::filesystem::path filePath = std::u8string { rawFilePath.begin(), rawFilePath.end() };

        std::ifstream stream { filePath };
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

    std::string_view Editor::hierarchyTypeToString(HierarchyType type)
    {
        switch (type) {
            case HierarchyType::Folder:
                return "Folder";
            case HierarchyType::Model:
                return "Model";
            case HierarchyType::PointLight:
                return "Point Light";
            case HierarchyType::SpotLight:
                return "Spot Light";
            default:
                return "???";
        }
    }

    entt::entity Editor::insertNewRootNode(HierarchyType type)
    {
        const auto entity = sceneHierarchy.components.create();

        auto& hierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(entity);
        hierarchyComponent.parent = entt::null;
        hierarchyComponent.type = type;
        hierarchyComponent.tag = hierarchyTypeToString(type);

        sceneHierarchy.components.emplace<EntityRoot>(entity);

        return entity;
    }

    entt::entity Editor::insertNewChildNode(entt::entity parentEntity, HierarchyComponent& parentComponent, HierarchyType type)
    {
        const auto child = sceneHierarchy.components.create();

        entt::entity previousChild = parentComponent.firstChild;
        parentComponent.firstChild = child;

        auto& childHierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(child);
        childHierarchyComponent.parent = parentEntity;
        childHierarchyComponent.next = previousChild;
        childHierarchyComponent.type = type;
        childHierarchyComponent.tag = hierarchyTypeToString(type);

        if (auto* previousChildHierarchyComponent = sceneHierarchy.components.try_get<HierarchyComponent>(previousChild)) {
            previousChildHierarchyComponent->previous = child;
        }

        return child;
    }

    void Editor::removeChilds(entt::entity childEntity)
    {
        while (childEntity != entt::null) {
            auto& childComponent = sceneHierarchy.components.get<HierarchyComponent>(childEntity);

            releasePointLightIndex(childEntity);
            releaseSpotLightIndex(childEntity);

            removeChilds(childComponent.firstChild);
            childComponent.firstChild = entt::null;

            entt::entity nextChild = childComponent.next;
            sceneHierarchy.components.destroy(childEntity);

            childEntity = nextChild;
        }
    }

    void Editor::drawHierarchyTree(entt::entity entity)
    {
        auto& sceneRegistry = sceneHierarchy.components;

        while (entity != entt::null) {
            auto& hierachyComponent = sceneRegistry.get<HierarchyComponent>(entity);
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

            if (hierachyComponent.firstChild == entt::null) {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            // ImGui uses void* as key for it lookup table, and entt have it all objects unique, which makes it perfect to be used as key
            auto* pointerID = reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<std::underlying_type_t<entt::entity>>(entity)));
            bool opened =
                ImGui::TreeNode(pointerID, hierachyComponent.tag.data(), hierachyComponent.tag.data() + hierachyComponent.tag.size(), flags);

            if (ImGui::BeginPopupContextItem(nullptr, ImGuiMouseButton_Right)) {
                ImGui::MenuItem(hierarchyTypeToString(hierachyComponent.type).data(), nullptr, nullptr, false);
                ImGui::Separator();

                if (ImGui::MenuItem("New Folder")) {
                    static_cast<void>(insertNewChildNode(entity, hierachyComponent, HierarchyType::Folder));
                }

                if (ImGui::MenuItem("New Model")) {
                    static_cast<void>(insertNewChildNode(entity, hierachyComponent, HierarchyType::Model));

                    // TODO: sceneRegistry.emplace<Model>(); ?
                }

                if (ImGui::MenuItem("New Point Light", nullptr, nullptr, pointLightCount < freePointLightIndices.size())) {
                    acquirePointLightIndex(insertNewChildNode(entity, hierachyComponent, HierarchyType::PointLight));
                }

                if (ImGui::MenuItem("New Spot Light", nullptr, nullptr, spotLightCount < freeSpotLightIndices.size())) {
                    acquireSpotLightIndex(insertNewChildNode(entity, hierachyComponent, HierarchyType::SpotLight));
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
                        auto& previousHierarchyComponent = sceneRegistry.get<HierarchyComponent>(hierachyComponent.previous);

                        if (hierachyComponent.next != entt::null) {
                            sceneRegistry.get<HierarchyComponent>(hierachyComponent.next).previous = hierachyComponent.previous;
                        }

                        previousHierarchyComponent.next = entt::null;
                    }
                    else if (hierachyComponent.parent != entt::null) {
                        sceneRegistry.get<HierarchyComponent>(hierachyComponent.parent).firstChild = hierachyComponent.next;
                    }

                    releasePointLightIndex(entity);
                    releaseSpotLightIndex(entity);

                    removeChilds(hierachyComponent.firstChild);
                    hierachyComponent.firstChild = entt::null;
                    sceneRegistry.destroy(entity);

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

    void Editor::acquirePointLightIndex(entt::entity entity)
    {
        auto& pointLight = sceneHierarchy.components.emplace<PointLightIndex>(entity);
        auto* pointLightsArray = gameHandle.pointLights->memory<PointLight*>();

        for (size_t index = 0; index < freePointLightIndices.size(); index++) {
            if (freePointLightIndices[index]) {
                pointLight.index = static_cast<uint32_t>(index);
                freePointLightIndices[index] = false;
                pointLightCount++;

                break;
            }
        }

        pointLightsArray[pointLight.index] = {
            .position = gameHandle.viewerObject.translation,
            .radius = 3.0f,
            .color = glm::vec3 { 1.0f },
        };
    }

    void Editor::releasePointLightIndex(entt::entity entity)
    {
        if (auto* pointLight = sceneHierarchy.components.try_get<PointLightIndex>(entity)) {
            auto* pointLightsArray = gameHandle.pointLights->memory<PointLight*>();
            pointLightsArray[pointLight->index] = {};

            freePointLightIndices[pointLight->index] = true;
            pointLightCount--;
        }
    }

    void Editor::acquireSpotLightIndex(entt::entity entity)
    {
        auto& spotLight = sceneHierarchy.components.emplace<SpotLightIndex>(entity);
        auto* spotLightsArray = gameHandle.spotLights->memory<SpotLight*>();

        for (size_t index = 0; index < freeSpotLightIndices.size(); index++) {
            if (freeSpotLightIndices[index]) {
                spotLight.index = static_cast<uint32_t>(index);
                freeSpotLightIndices[index] = false;
                spotLightCount++;

                break;
            }
        }

        spotLightsArray[spotLight.index] = {
            .position = gameHandle.viewerObject.translation,
            .radius = 3.0f,
            .color = glm::vec3 { 1.0f },
            .coneDirection = glm::vec3 { 0.0f, -1.0f, 0.0f },
            .coneAngle = 30.0f,
        };
    }

    void Editor::releaseSpotLightIndex(entt::entity entity)
    {
        if (auto* spotLight = sceneHierarchy.components.try_get<SpotLightIndex>(entity)) {
            auto* spotLightsArray = gameHandle.spotLights->memory<SpotLight*>();
            spotLightsArray[spotLight->index] = {};

            freeSpotLightIndices[spotLight->index] = true;
            spotLightCount--;
        }
    }

    void Editor::outputFramerateInfo(const std::string_view& name, const float (&buffer)[SceneViewport::kAverageStatisticBufferSize])
    {
        constexpr uint32_t kGreenColor = IM_COL32(0, 255, 0, 255);

        const std::string formattedTime = std::vformat(name, std::make_format_args(calculateAverage(buffer)));
        ImGui::GetWindowDrawList()
            ->AddText(sceneViewport.framerateTextPosition, kGreenColor, formattedTime.data(), formattedTime.data() + formattedTime.size());

        sceneViewport.framerateTextPosition.y += 15;
    }

    void Editor::outputMemoryUsage(const std::array<VmaBudget, VK_MAX_MEMORY_HEAPS>& budgets, const std::string_view& name, int32_t heapIndex)
    {
        constexpr uint32_t kGreenColor = IM_COL32(0, 255, 0, 255);

        if (heapIndex != SceneViewport::kInvalidHeapIndex) {
            const VmaBudget& budget = budgets[heapIndex];

            const float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.0f;
            const float usageInMB = budget.usage / bytesToMegabytes;
            const float budgetInMB = budget.budget / bytesToMegabytes;
            sceneViewport.framerateTextPosition.y += 15;

            std::string formatted = std::vformat("{}: {:.2f}/{:.2f} MB ({:.2f}%)", std::make_format_args(name, usageInMB, budgetInMB, percentage));
            ImGui::GetWindowDrawList()
                ->AddText(sceneViewport.framerateTextPosition, kGreenColor, formatted.data(), formatted.data() + formatted.size());
        }
    }

} // namespace yorna