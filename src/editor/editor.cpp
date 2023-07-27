#include <editor/editor.hpp>

#include <editor/extensions.hpp>
#include <editor/imgui.hpp>
#include <editor/localization.hpp>
#include <entities/light_objects.hpp>

#include <coffee/objects/vertex.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Imguizmo.h>
#include <json.hpp>

namespace yorna {

    constexpr float kBytesToMegabytes = 1000000.0f;
    constexpr float kDirectorySpacingSize = 96.0f;
    constexpr uint32_t kSpecialAffectedModelIndex = 0xFFFFFFFF;

    Editor::Editor(const SharedInstance& instance, yorna::Yorna& gameHandle) noexcept : SharedInstance { instance }, gameHandle { gameHandle }
    {
        initializeLights();
        initializeMemoryHeaps();
        initializeMousePicking();

        projectInformation.dialog.SetLocales(LC_ALL, "ru_RU.UTF-8", "C");

        framebufferImage = gameHandle.outputDescriptor;
    }

    void Editor::update()
    {
        if (updateInformation.modelWasChanged && gizmo.affectedModel != nullptr) {
            gizmo.affectedModel->decompose(updateInformation.transformationMatrix);
        }
    }

    void Editor::render()
    {
        ImGui::NewFrame();

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

                    hierarchy.active = false;
                    scene.active = false;
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
                ImGui::MenuItem(localizeName(LocaleName::SceneHierarchy), nullptr, &hierarchy.active, projectInformation.loaded);
                ImGui::MenuItem(localizeName(LocaleName::GizmoViewport), nullptr, &gizmo.active, projectInformation.loaded);
                ImGui::MenuItem(localizeName(LocaleName::SceneViewport), nullptr, &scene.active, projectInformation.loaded);
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

        if (hierarchy.active) {
            std::string sceneHierarchyName = std::format("{}###HierarchyViewport", localizeName(LocaleName::SceneHierarchy));
            ImGui::Begin(sceneHierarchyName.data(), &hierarchy.active);

            if (ImGui::BeginPopupContextWindow("SceneHierarchyRightClick", ImGuiPopupFlags_MouseButtonRight)) {
                if (ImGui::MenuItem("New Folder")) {
                    static_cast<void>(insertNewRootNode(HierarchyType::Folder));
                }

                if (ImGui::MenuItem("New Model")) {
                    static_cast<void>(insertNewRootNode(HierarchyType::Model));

                    // TODO: hierarchy.components.emplace<Model>(); ?
                }

                if (ImGui::MenuItem("New Point Light", nullptr, nullptr, pointLightCount < freePointLightIndices.size())) {
                    acquirePointLightIndex(insertNewRootNode(HierarchyType::PointLight));
                }

                if (ImGui::MenuItem("New Spot Light", nullptr, nullptr, spotLightCount < freeSpotLightIndices.size())) {
                    acquireSpotLightIndex(insertNewRootNode(HierarchyType::SpotLight));
                }

                ImGui::EndPopup();
            }

            for (auto entity : hierarchy.components.view<EntityRoot>()) {
                drawHierarchyTree(entity);
            }

            ImGui::End();
        }

        if (gizmo.active) {
            std::string guizmoViewportName = std::format("{}###GuizmoViewport", localizeName(LocaleName::GizmoViewport));
            ImGui::Begin(guizmoViewportName.data(), &gizmo.active);

            if (gizmo.affectedModel != nullptr) {
                if (ImGui::RadioButton("Translate", gizmo.operation == ImGuizmo::TRANSLATE)) {
                    gizmo.operation = ImGuizmo::TRANSLATE;
                }
                ImGui::SameLine();

                if (ImGui::RadioButton("Rotate", gizmo.operation == ImGuizmo::ROTATE)) {
                    gizmo.operation = ImGuizmo::ROTATE;
                }
                ImGui::SameLine();

                if (ImGui::RadioButton("Scale", gizmo.operation == ImGuizmo::SCALE)) {
                    gizmo.operation = ImGuizmo::SCALE;
                }

                constexpr ImGuiInputTextFlags kInputFlags = ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal;

                ImGui::InputFloat3("Tr", glm::value_ptr(gizmo.affectedModel->translation), "%.3f", kInputFlags);
                ImGui::InputFloat3("Rt", glm::value_ptr(gizmo.affectedModel->rotation), "%.3f", kInputFlags);
                ImGui::InputFloat3("Sc", glm::value_ptr(gizmo.affectedModel->scale), "%.3f", kInputFlags);
            }

            ImGui::End();
        }

        if (scene.active) {
            std::string sceneViewportName = std::format("{}###SceneViewport", localizeName(LocaleName::SceneViewport));
            ImGui::Begin(sceneViewportName.data(), &scene.active);

            ImGui::Checkbox(localizeName(LocaleName::KeepAspectRatio), &scene.keepAspectRatio);
            ImGui::SameLine();
            ImGui::Checkbox(localizeName(LocaleName::DisplayFrametimeFPS), &scene.outputFramerateAndFPS);

            ImGuiViewport* viewport = ImGui::GetWindowViewport();
            ImVec2 currentCursorPosition = ImGui::GetCursorScreenPos();
            scene.size = ImGui::GetContentRegionAvail();

            float newAspectRatio = scene.keepAspectRatio ? kDefaultAspectRatio : scene.size.x / scene.size.y;
            gameHandle.outputAspect.store(newAspectRatio, std::memory_order_relaxed);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });
            ImGui::Image(&framebufferImage, scene.size);
            ImGui::PopStyleVar();

            if (!ImGuizmo::IsOver()) {
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    doMousePicking(currentCursorPosition);
                }

                if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    boundViewportToGame(viewport);
                }
                else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                    unboundViewportFromGame(viewport);
                }
            }

            if (gizmo.waitingForModelSelection && gizmo.completionFence->status() == VK_SUCCESS) {
                gizmo.pickingOutput->invalidate();

                uint32_t affectedModelIndex = gizmo.pickingOutput->memory<uint32_t>();
                gizmo.affectedModel = affectedModelIndex == kSpecialAffectedModelIndex ? nullptr : &gameHandle.models[affectedModelIndex]->transform;

                gizmo.waitingForModelSelection = false;
            }

            if (gizmo.affectedModel != nullptr) {
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(currentCursorPosition.x, currentCursorPosition.y, scene.size.x, scene.size.y);

                const float* view = glm::value_ptr(gameHandle.camera.view());
                const float* projection = glm::value_ptr(gameHandle.camera.projection());
                updateInformation.transformationMatrix = gizmo.affectedModel->mat4();

                // State will be changed directly inside UpdateInformation struct, so we don't need to do any additional checks
                ImGuizmo::Manipulate(view, projection, gizmo.operation, ImGuizmo::LOCAL, glm::value_ptr(updateInformation.transformationMatrix));
                updateInformation.modelWasChanged = ImGuizmo::IsUsing();
            }

            if (scene.outputFramerateAndFPS) {
                scene.framerateTextPosition = { currentCursorPosition.x + 10, currentCursorPosition.y + 10 };

                outputFramerateInfo("{:.3f} ms/frame", scene.averageFrameTime);
                outputFramerateInfo("{:.1f} FPS", scene.averageFPS);
                outputFramerateInfo("ImGUI CPU: {:.6f} ms", scene.averageImGuiTime);
                outputFramerateInfo("Depth pass: {:.6f} ms", scene.averageDepthPass);
                outputFramerateInfo("Light pass: {:.6f} ms", scene.averageLightPass);
                outputFramerateInfo("Rendering: {:.6f} ms", scene.averageRendering);

                std::array<VmaBudget, VK_MAX_MEMORY_HEAPS> budgets = device->heapBudgets();

                outputMemoryUsage(budgets, "Video RAM", scene.deviceHeapIndex);
                outputMemoryUsage(budgets, "Host RAM", scene.hostHeapIndex);
                outputMemoryUsage(budgets, "Shared RAM", scene.sharedHeapIndex);
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
        scene.averageFPS[scene.statisticIndex] = 1.0f / deltaTime;
        scene.averageFrameTime[scene.statisticIndex] = 1000.0f * deltaTime;
        scene.averageImGuiTime[scene.statisticIndex] = imguiTiming;

        std::vector<float> gpuTimings = queryTimestamps.extractResults();

        if (gpuTimings.back() > 0.0f) {
            scene.averageDepthPass[scene.statisticIndex] = gpuTimings[0];
            scene.averageLightPass[scene.statisticIndex] = gpuTimings[1];
            scene.averageRendering[scene.statisticIndex] = gpuTimings[2];
        }

        scene.statisticIndex = (scene.statisticIndex + 1) % SceneViewport::kAverageStatisticBufferSize;
    }

    void Editor::initializeLights()
    {
        freePointLightIndices.fill(true);
        freeSpotLightIndices.fill(true);
    }

    void Editor::initializeMemoryHeaps()
    {
        const VkPhysicalDeviceMemoryProperties& properties = device->memoryProperties();

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
            const VkMemoryType& memoryType = properties.memoryTypes[i];

            bool isDeviceMemory = (memoryType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0;
            bool isHostMemory = (memoryType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;

            if (isDeviceMemory && isHostMemory) {
                scene.sharedHeapIndex = memoryType.heapIndex;
            }
            else if (isDeviceMemory) {
                scene.deviceHeapIndex = memoryType.heapIndex;
            }
            else if (isHostMemory) {
                scene.hostHeapIndex = memoryType.heapIndex;
            }
        }
    }

    // clang-format off

    void Editor::initializeMousePicking()
    {
        gizmo.pickingOutput = coffee::graphics::Buffer::create(device, {
            .instanceSize = static_cast<uint32_t>(sizeof(uint32_t)),
            .instanceCount = 1U,
            .usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            .allocationUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
        });

        gizmo.renderPass = coffee::graphics::RenderPass::create(device, {
            .depthStencilAttachment = {
                .format = device->optimalDepthFormat(),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            }
        });

        gizmo.layout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT    } },
            { 1, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT  } }
        });

        auto writer = coffee::graphics::DescriptorWriter(gizmo.layout);

        for (size_t index = 0; index < coffee::graphics::Device::kMaxOperationsInFlight; index++) {
            writer.addBuffer(0, mvpUniformBuffers[index]);
            writer.addBuffer(1, gizmo.pickingOutput);

            gizmo.descriptors[index] = coffee::graphics::DescriptorSet::create(device, writer);
        }

        gizmo.pipeline = coffee::graphics::GraphicsPipeline::create(device, gizmo.renderPass, {
            .vertexShader = assetManager->getShader(filesystem, "shaders/mouse_picking.vert.spv"),
            .fragmentShader = assetManager->getShader(filesystem, "shaders/mouse_picking.frag.spv"),
            .vertexPushConstants = {
                .size = sizeof(MousePickingPushConstants),
                .offset = 0ULL
            },
            .layouts = {
                gizmo.layout
            },
            .inputBindings = { coffee::graphics::InputBinding {
                .binding = 0,
                .stride = sizeof(coffee::Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = { coffee::graphics::InputElement {
                    .location = 0U,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = offsetof(coffee::Vertex, position),
                }},
            }},
            .inputAssembly = {
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = false,
            },
            .rasterizationInfo = {
                .cullMode = VK_CULL_MODE_BACK_BIT,
            },
            .depthStencilInfo = {
                .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
            },
        });

        gizmo.completionFence = coffee::graphics::Fence::create(device, true);
    }

    void Editor::resizeMousePicking() {
        if (gizmo.depthImage != nullptr) {
            auto& extent = gizmo.depthImage->extent;

            if (extent.width == scene.size.x && extent.height == scene.size.y) {
                return;
            }
        }

        gizmo.depthImage = coffee::graphics::Image::create(device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = device->optimalDepthFormat(),
            .extent = { static_cast<uint32_t>(scene.size.x), static_cast<uint32_t>(scene.size.y), 1U },
            .mipLevels = 1U,
            .arrayLayers = 1U,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .priority = 1.0f,
        });

        gizmo.depthImageView = coffee::graphics::ImageView::create(gizmo.depthImage, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = gizmo.depthImage->imageFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        });

        gizmo.framebuffer = coffee::graphics::Framebuffer::create(device, gizmo.renderPass, {
            .extent = { static_cast<uint32_t>(scene.size.x), static_cast<uint32_t>(scene.size.y) },
            .depthStencilView = gizmo.depthImageView
        });
    }

    // clang-format on

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
        const auto entity = hierarchy.components.create();

        auto& hierarchyComponent = hierarchy.components.emplace<HierarchyComponent>(entity);
        hierarchyComponent.parent = entt::null;
        hierarchyComponent.type = type;
        hierarchyComponent.tag = hierarchyTypeToString(type);

        hierarchy.components.emplace<EntityRoot>(entity);

        return entity;
    }

    entt::entity Editor::insertNewChildNode(entt::entity parentEntity, HierarchyComponent& parentComponent, HierarchyType type)
    {
        const auto child = hierarchy.components.create();

        entt::entity previousChild = parentComponent.firstChild;
        parentComponent.firstChild = child;

        auto& childHierarchyComponent = hierarchy.components.emplace<HierarchyComponent>(child);
        childHierarchyComponent.parent = parentEntity;
        childHierarchyComponent.next = previousChild;
        childHierarchyComponent.type = type;
        childHierarchyComponent.tag = hierarchyTypeToString(type);

        if (auto* previousChildHierarchyComponent = hierarchy.components.try_get<HierarchyComponent>(previousChild)) {
            previousChildHierarchyComponent->previous = child;
        }

        return child;
    }

    void Editor::removeChilds(entt::entity childEntity)
    {
        while (childEntity != entt::null) {
            auto& childComponent = hierarchy.components.get<HierarchyComponent>(childEntity);

            releasePointLightIndex(childEntity);
            releaseSpotLightIndex(childEntity);

            removeChilds(childComponent.firstChild);
            childComponent.firstChild = entt::null;

            entt::entity nextChild = childComponent.next;
            hierarchy.components.destroy(childEntity);

            childEntity = nextChild;
        }
    }

    void Editor::drawHierarchyTree(entt::entity entity)
    {
        auto& sceneRegistry = hierarchy.components;

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
                    hierarchy.renameEntity = entity;
                }

                if (ImGui::MenuItem("Delete")) {
                    hierarchy.deleteEntity = entity;
                }

                ImGui::EndPopup();
            }

            if (hierarchy.renameEntity == entity) {
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
                    hierarchy.renameEntity = entt::null;
                }

                ImGui::EndPopup();
            }

            if (hierarchy.deleteEntity == entity) {
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
                    hierarchy.deleteEntity = entt::null;
                }

                ImGui::SameLine();

                if (ImGui::Button("No", { 64.0f, 0.0f })) {
                    ImGui::CloseCurrentPopup();
                    hierarchy.deleteEntity = entt::null;
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

    void Editor::doMousePicking(const ImVec2& currentCursorPosition)
    {
        ImVec2 mousePosition = ImGui::GetMousePos();

        // clang-format off
        const VkRect2D renderArea = { .extent = {
            .width = static_cast<uint32_t>(scene.size.x),
            .height = static_cast<uint32_t>(scene.size.y),
        } };
        const VkRect2D scissor = {
            .offset = {
                .x = static_cast<int32_t>(std::clamp(mousePosition.x - currentCursorPosition.x, 0.0f, scene.size.x - 1.0f)),
                .y = static_cast<int32_t>(std::clamp(mousePosition.y - currentCursorPosition.y, 0.0f, scene.size.y - 1.0f)),
            },
            .extent = {
                .width = 1U,
                .height = 1U,
            },
        };
        const VkViewport viewport = {
            .x = 0,
            .y = 0,
            .width = scene.size.x,
            .height = scene.size.y,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        constexpr VkClearValue clearValues =  { .depthStencil = { 0.0f, 0U } };
        // clang-format on

        gizmo.completionFence->wait();
        gizmo.completionFence->reset();

        resizeMousePicking();

        gizmo.pickingOutput->memory<uint32_t>() = kSpecialAffectedModelIndex;
        gizmo.pickingOutput->flush();

        auto mousePickingCommandBuffer = coffee::graphics::CommandBuffer::createGraphics(device);

        mousePickingCommandBuffer.beginRenderPass(gizmo.renderPass, gizmo.framebuffer, renderArea, 1U, &clearValues);
        mousePickingCommandBuffer.setScissor(scissor);
        mousePickingCommandBuffer.setViewport(viewport);
        mousePickingCommandBuffer.bindPipeline(gizmo.pipeline);
        mousePickingCommandBuffer.bindDescriptorSets(gizmo.pipeline, gizmo.descriptors[gameHandle.currentFrame()]);

        for (size_t index = 0; index < gameHandle.models.size(); index++) {
            auto& model = gameHandle.models[index];
            auto& meshes = model->model->meshes;
            auto& visibleMeshes = model->visibleMeshes;

            MousePickingPushConstants pushConstants {
                .modelMatrix = model->transform.mat4(),
                .objectID = static_cast<uint32_t>(index),
            };

            mousePickingCommandBuffer.pushConstants(gizmo.pipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstants), &pushConstants);
            mousePickingCommandBuffer.bindModel(model->model);

            for (size_t meshID : visibleMeshes) {
                mousePickingCommandBuffer.drawMesh(meshes[meshID]);
            }
        }

        mousePickingCommandBuffer.endRenderPass();

        device->submit(std::move(mousePickingCommandBuffer), {}, gizmo.completionFence);
        gizmo.waitingForModelSelection = true;
    }

    void Editor::boundViewportToGame(ImGuiViewport* viewport)
    {
        if (!scene.cameraCaptured) {
            ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
            coffee::graphics::Window* windowHandle = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;

            backendData->fullControlWindowPtr = windowHandle;
            windowHandle->disableCursor();

            scene.cameraCaptured = true;
        }
    }

    void Editor::unboundViewportFromGame(ImGuiViewport* viewport)
    {
        if (scene.cameraCaptured) {
            ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
            coffee::graphics::Window* windowHandle = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;

            backendData->fullControlWindowPtr = nullptr;
            windowHandle->showCursor();

            scene.cameraCaptured = false;
        }
    }

    void Editor::acquirePointLightIndex(entt::entity entity)
    {
        auto& pointLight = hierarchy.components.emplace<PointLightIndex>(entity);
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
        if (auto* pointLight = hierarchy.components.try_get<PointLightIndex>(entity)) {
            auto* pointLightsArray = gameHandle.pointLights->memory<PointLight*>();
            pointLightsArray[pointLight->index] = {};

            freePointLightIndices[pointLight->index] = true;
            pointLightCount--;
        }
    }

    void Editor::acquireSpotLightIndex(entt::entity entity)
    {
        auto& spotLight = hierarchy.components.emplace<SpotLightIndex>(entity);
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
        if (auto* spotLight = hierarchy.components.try_get<SpotLightIndex>(entity)) {
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
            ->AddText(scene.framerateTextPosition, kGreenColor, formattedTime.data(), formattedTime.data() + formattedTime.size());

        scene.framerateTextPosition.y += 15;
    }

    void Editor::outputMemoryUsage(const std::array<VmaBudget, VK_MAX_MEMORY_HEAPS>& budgets, const std::string_view& name, int32_t heapIndex)
    {
        constexpr uint32_t kGreenColor = IM_COL32(0, 255, 0, 255);

        if (heapIndex != SceneViewport::kInvalidHeapIndex) {
            const VmaBudget& budget = budgets[heapIndex];

            const float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.0f;
            const float usageInMB = budget.usage / kBytesToMegabytes;
            const float budgetInMB = budget.budget / kBytesToMegabytes;
            scene.framerateTextPosition.y += 15;

            std::string formatted = std::vformat("{}: {:.2f}/{:.2f} MB ({:.2f}%)", std::make_format_args(name, usageInMB, budgetInMB, percentage));
            ImGui::GetWindowDrawList()->AddText(scene.framerateTextPosition, kGreenColor, formatted.data(), formatted.data() + formatted.size());
        }
    }

} // namespace yorna