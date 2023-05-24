#include <systems/imgui/system.hpp>

#include <coffee/coffee.hpp>

#include <fstream>

#include <systems/imgui/extensions.hpp>
#include <systems/imgui/fira_code.hpp>
#include <systems/imgui/localization.hpp>
#include <systems/imgui/shaders.hpp>
#include <systems/imgui/structures.hpp>

#include <oneapi/tbb/task_group.h>
#include <json.hpp>

namespace editor {

    constexpr uint32_t timestampQueryCount = 4U;

    constexpr std::array<VkClearValue, 1> clearValues = { VkClearValue { .color = { 0.0f, 0.0f, 0.0f, 1.0f } } };

    constexpr VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    ImGuiKey keyToImGui(coffee::Keys key)
    {
        switch (key) {
            case coffee::Keys::Tab:
                return ImGuiKey_Tab;
            case coffee::Keys::Left:
                return ImGuiKey_LeftArrow;
            case coffee::Keys::Right:
                return ImGuiKey_RightArrow;
            case coffee::Keys::Up:
                return ImGuiKey_UpArrow;
            case coffee::Keys::Down:
                return ImGuiKey_DownArrow;
            case coffee::Keys::PageUp:
                return ImGuiKey_PageUp;
            case coffee::Keys::PageDown:
                return ImGuiKey_PageDown;
            case coffee::Keys::Home:
                return ImGuiKey_Home;
            case coffee::Keys::End:
                return ImGuiKey_End;
            case coffee::Keys::Insert:
                return ImGuiKey_Insert;
            case coffee::Keys::Delete:
                return ImGuiKey_Delete;
            case coffee::Keys::Backspace:
                return ImGuiKey_Backspace;
            case coffee::Keys::Space:
                return ImGuiKey_Space;
            case coffee::Keys::Enter:
                return ImGuiKey_Enter;
            case coffee::Keys::Escape:
                return ImGuiKey_Escape;
            case coffee::Keys::Apostrophe:
                return ImGuiKey_Apostrophe;
            case coffee::Keys::Comma:
                return ImGuiKey_Comma;
            case coffee::Keys::Minus:
                return ImGuiKey_Minus;
            case coffee::Keys::Period:
                return ImGuiKey_Period;
            case coffee::Keys::Slash:
                return ImGuiKey_Slash;
            case coffee::Keys::Semicolon:
                return ImGuiKey_Semicolon;
            case coffee::Keys::Equal:
                return ImGuiKey_Equal;
            case coffee::Keys::LeftBracket:
                return ImGuiKey_LeftBracket;
            case coffee::Keys::Backslash:
                return ImGuiKey_Backslash;
            case coffee::Keys::RightBracket:
                return ImGuiKey_RightBracket;
            case coffee::Keys::GraveAccent:
                return ImGuiKey_GraveAccent;
            case coffee::Keys::CapsLock:
                return ImGuiKey_CapsLock;
            case coffee::Keys::ScrollLock:
                return ImGuiKey_ScrollLock;
            case coffee::Keys::NumLock:
                return ImGuiKey_NumLock;
            case coffee::Keys::PrintScreen:
                return ImGuiKey_PrintScreen;
            case coffee::Keys::Pause:
                return ImGuiKey_Pause;
            case coffee::Keys::KP0:
                return ImGuiKey_Keypad0;
            case coffee::Keys::KP1:
                return ImGuiKey_Keypad1;
            case coffee::Keys::KP2:
                return ImGuiKey_Keypad2;
            case coffee::Keys::KP3:
                return ImGuiKey_Keypad3;
            case coffee::Keys::KP4:
                return ImGuiKey_Keypad4;
            case coffee::Keys::KP5:
                return ImGuiKey_Keypad5;
            case coffee::Keys::KP6:
                return ImGuiKey_Keypad6;
            case coffee::Keys::KP7:
                return ImGuiKey_Keypad7;
            case coffee::Keys::KP8:
                return ImGuiKey_Keypad8;
            case coffee::Keys::KP9:
                return ImGuiKey_Keypad9;
            case coffee::Keys::KPDecimal:
                return ImGuiKey_KeypadDecimal;
            case coffee::Keys::KPDivide:
                return ImGuiKey_KeypadDivide;
            case coffee::Keys::KPMultiply:
                return ImGuiKey_KeypadMultiply;
            case coffee::Keys::KPSubtract:
                return ImGuiKey_KeypadSubtract;
            case coffee::Keys::KPAdd:
                return ImGuiKey_KeypadAdd;
            case coffee::Keys::KPEnter:
                return ImGuiKey_KeypadEnter;
            case coffee::Keys::KPEqual:
                return ImGuiKey_KeypadEqual;
            case coffee::Keys::LeftShift:
                return ImGuiKey_LeftShift;
            case coffee::Keys::LeftControl:
                return ImGuiKey_LeftCtrl;
            case coffee::Keys::LeftAlt:
                return ImGuiKey_LeftAlt;
            case coffee::Keys::LeftSuper:
                return ImGuiKey_LeftSuper;
            case coffee::Keys::RightShift:
                return ImGuiKey_RightShift;
            case coffee::Keys::RightControl:
                return ImGuiKey_RightCtrl;
            case coffee::Keys::RightAlt:
                return ImGuiKey_RightAlt;
            case coffee::Keys::RightSuper:
                return ImGuiKey_RightSuper;
            case coffee::Keys::Menu:
                return ImGuiKey_Menu;
            case coffee::Keys::D0:
                return ImGuiKey_0;
            case coffee::Keys::D1:
                return ImGuiKey_1;
            case coffee::Keys::D2:
                return ImGuiKey_2;
            case coffee::Keys::D3:
                return ImGuiKey_3;
            case coffee::Keys::D4:
                return ImGuiKey_4;
            case coffee::Keys::D5:
                return ImGuiKey_5;
            case coffee::Keys::D6:
                return ImGuiKey_6;
            case coffee::Keys::D7:
                return ImGuiKey_7;
            case coffee::Keys::D8:
                return ImGuiKey_8;
            case coffee::Keys::D9:
                return ImGuiKey_9;
            case coffee::Keys::A:
                return ImGuiKey_A;
            case coffee::Keys::B:
                return ImGuiKey_B;
            case coffee::Keys::C:
                return ImGuiKey_C;
            case coffee::Keys::D:
                return ImGuiKey_D;
            case coffee::Keys::E:
                return ImGuiKey_E;
            case coffee::Keys::F:
                return ImGuiKey_F;
            case coffee::Keys::G:
                return ImGuiKey_G;
            case coffee::Keys::H:
                return ImGuiKey_H;
            case coffee::Keys::I:
                return ImGuiKey_I;
            case coffee::Keys::J:
                return ImGuiKey_J;
            case coffee::Keys::K:
                return ImGuiKey_K;
            case coffee::Keys::L:
                return ImGuiKey_L;
            case coffee::Keys::M:
                return ImGuiKey_M;
            case coffee::Keys::N:
                return ImGuiKey_N;
            case coffee::Keys::O:
                return ImGuiKey_O;
            case coffee::Keys::P:
                return ImGuiKey_P;
            case coffee::Keys::Q:
                return ImGuiKey_Q;
            case coffee::Keys::R:
                return ImGuiKey_R;
            case coffee::Keys::S:
                return ImGuiKey_S;
            case coffee::Keys::T:
                return ImGuiKey_T;
            case coffee::Keys::U:
                return ImGuiKey_U;
            case coffee::Keys::V:
                return ImGuiKey_V;
            case coffee::Keys::W:
                return ImGuiKey_W;
            case coffee::Keys::X:
                return ImGuiKey_X;
            case coffee::Keys::Y:
                return ImGuiKey_Y;
            case coffee::Keys::Z:
                return ImGuiKey_Z;
            case coffee::Keys::F1:
                return ImGuiKey_F1;
            case coffee::Keys::F2:
                return ImGuiKey_F2;
            case coffee::Keys::F3:
                return ImGuiKey_F3;
            case coffee::Keys::F4:
                return ImGuiKey_F4;
            case coffee::Keys::F5:
                return ImGuiKey_F5;
            case coffee::Keys::F6:
                return ImGuiKey_F6;
            case coffee::Keys::F7:
                return ImGuiKey_F7;
            case coffee::Keys::F8:
                return ImGuiKey_F8;
            case coffee::Keys::F9:
                return ImGuiKey_F9;
            case coffee::Keys::F10:
                return ImGuiKey_F10;
            case coffee::Keys::F11:
                return ImGuiKey_F11;
            case coffee::Keys::F12:
                return ImGuiKey_F12;
            default:
                return ImGuiKey_None;
        }
    }

    ImGuiSystem::ImGuiSystem(const coffee::GPUDevicePtr& device, coffee::LoopHandler& loopHandler)
        : device { device }
        , loopHandler { loopHandler }
    {
        applicationWindow = coffee::Window::create(device, {
            .extent = { 1280U, 720U },
            .presentMode = presentMode
        });

        initializeImGui();
        initializeBackend();

        framebufferImage = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->gameSystem->outputSet;

        projectInformation.dialog.SetLocales(LC_ALL, "ru_RU.UTF-8", "C");
    }

    ImGuiSystem::~ImGuiSystem()
    {
        vkDeviceWaitIdle(device->logicalDevice());

        destroyQueryPool();

        ImGui::DestroyPlatformWindows();

        delete static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        delete static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        ImGui::DestroyContext();
    }

    void ImGuiSystem::run()
    {
        loopHandler.setFramerateLimit(1440.0f);

        auto gameThreadWork = [this]() {
            BackendPlatformData* platformData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
            coffee::CommandBuffer commandBuffer = coffee::CommandBuffer::createGraphics(device);

            resetQueryPool(commandBuffer);
            platformData->gameSystem->update();

            beginTimestamp(commandBuffer, 0);
            platformData->gameSystem->performDepthTest(commandBuffer);
            endTimestamp(commandBuffer, 0);

            beginTimestamp(commandBuffer, 1);
            platformData->gameSystem->performRendering(commandBuffer);
            endTimestamp(commandBuffer, 1);

            device->sendCommandBuffer(std::move(commandBuffer));
        };

        tbb::task_group gameLoopTask {};

        applicationWindow->showWindow();
        std::chrono::system_clock::time_point loopBeginTime {};

        while (!applicationWindow->shouldClose()) {
            loopHandler.pollEvents();

            while (!isAnyWindowActive()) {
                loopHandler.pollEvents(1.0 / 60.0);
            }

            loopBeginTime = std::chrono::system_clock::now();

            this->update(loopHandler.deltaTime());

            gameLoopTask.run(gameThreadWork);
            this->prepareImGui();
            this->render();

            gameLoopTask.wait();
            device->submitPendingWork();

            sceneViewport.averageCPUTime[sceneViewport.statisticIndex] =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - loopBeginTime).count();

            loopHandler.waitFramelimit();

            sceneViewport.statisticIndex = (sceneViewport.statisticIndex + 1) % SceneViewport::averageStatisticBufferSize;
        }
    }

    void ImGuiSystem::update(float deltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();

        VkExtent2D windowSize = applicationWindow->windowSize();
        io.DisplaySize = ImVec2 { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };

        if (windowSize.width > 0 && windowSize.height > 0) {
            VkExtent2D framebufferSize = applicationWindow->framebufferSize();

            io.DisplayFramebufferScale = ImVec2 { static_cast<float>(windowSize.width / framebufferSize.width),
                                                  static_cast<float>(windowSize.height / framebufferSize.height) };
        }

        io.DeltaTime = deltaTime;

        if (static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors) {
            updateMonitors();
        }

        updateMouse(applicationWindow);
        updateCursor(applicationWindow);

        acquired = applicationWindow->acquireNextImage();
    }

    void ImGuiSystem::render()
    {
        ImGuiIO& io = ImGui::GetIO();

        if (acquired) {
            coffee::CommandBuffer commandBuffer = coffee::CommandBuffer::createGraphics(device);
            this->render(ImGui::GetMainViewport(), commandBuffer);
            applicationWindow->sendCommandBuffer(std::move(commandBuffer));
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiSystem::render(ImGuiViewport* viewport, const coffee::CommandBuffer& commandBuffer)
    {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
        ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
        RendererData* rendererData = static_cast<RendererData*>(viewport->RendererUserData);
        ImDrawData* data = viewport->DrawData;

        const uint32_t frameIndex = viewportData->windowHandle->currentImageIndex();
        float framebufferWidth = data->DisplaySize.x * data->FramebufferScale.x;
        float framebufferHeight = data->DisplaySize.y * data->FramebufferScale.y;

        if (framebufferWidth <= 0.0f || framebufferHeight <= 0.0f) {
            return;
        }

        commandBuffer.beginRenderPass(backendData->renderPass, rendererData->framebuffers[frameIndex], {
            .offset = { 0, 0 },
            .extent = viewportData->windowHandle->framebufferSize()
        }, static_cast<uint32_t>(clearValues.size()), clearValues.data());

        if (data->TotalVtxCount > 0) {
            size_t vertexSize = data->TotalVtxCount * sizeof(ImDrawVert);
            size_t indexSize = data->TotalIdxCount * sizeof(ImDrawIdx);

            coffee::BufferPtr& vertexBuffer = rendererData->vertexBuffers[frameIndex];
            coffee::BufferPtr& indexBuffer = rendererData->indexBuffers[frameIndex];

            if (vertexBuffer == nullptr || (vertexBuffer->instanceSize * vertexBuffer->instanceCount) < vertexSize) {
                if (vertexBuffer != nullptr) {
                    vertexBuffer->unmap();
                }

                coffee::BufferConfiguration configuration {};
                configuration.instanceSize = sizeof(ImDrawVert);
                configuration.instanceCount = data->TotalVtxCount;
                configuration.usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                configuration.allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

                vertexBuffer = coffee::Buffer::create(backendData->device, configuration);
                vertexBuffer->map();
            }

            if (indexBuffer == nullptr || (indexBuffer->instanceSize * indexBuffer->instanceCount) < indexSize) {
                if (indexBuffer != nullptr) {
                    indexBuffer->unmap();
                }

                coffee::BufferConfiguration configuration {};
                configuration.instanceSize = sizeof(ImDrawIdx);
                configuration.instanceCount = data->TotalIdxCount;
                configuration.usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                configuration.allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

                indexBuffer = coffee::Buffer::create(backendData->device, configuration);
                indexBuffer->map();
            }

            size_t vertexOffset = 0;
            size_t indexOffset = 0;

            for (size_t i = 0; i < data->CmdListsCount; i++) {
                const ImDrawList* list = data->CmdLists[i];

                std::memcpy(static_cast<char*>(vertexBuffer->memory()) + vertexOffset,
                    list->VtxBuffer.Data, list->VtxBuffer.Size * sizeof(ImDrawVert));
                std::memcpy(static_cast<char*>(indexBuffer->memory()) + indexOffset,
                    list->IdxBuffer.Data, list->IdxBuffer.Size * sizeof(ImDrawIdx));

                vertexOffset += list->VtxBuffer.Size * sizeof(ImDrawVert);
                indexOffset += list->IdxBuffer.Size * sizeof(ImDrawIdx);
            }

            VkDeviceSize offsets[] = { 0 };
            commandBuffer.bindVertexBuffers(1, &vertexBuffer->buffer(), offsets);
            commandBuffer.bindIndexBuffer(indexBuffer);
        }

        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, backendData->pipeline);
        commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, backendData->pipeline, 1, &backendData->descriptorSet->set());
        commandBuffer.setViewport({ .width = framebufferWidth, .height = framebufferHeight, .minDepth = 0.0f, .maxDepth = 1.0f });

        ImGuiPushConstant constants {};
        constants.scale.x = 2.0f / data->DisplaySize.x;
        constants.scale.y = 2.0f / data->DisplaySize.y;
        constants.translate.x = -1.0f - data->DisplayPos.x * constants.scale.x;
        constants.translate.y = -1.0f - data->DisplayPos.y * constants.scale.y;
        commandBuffer.pushConstants(backendData->pipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(ImGuiPushConstant), &constants);

        ImVec2 clipOff = data->DisplayPos;
        ImVec2 clipScale = data->FramebufferScale;

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;

        for (int i = 0; i < data->CmdListsCount; i++) {
            const ImDrawList* list = data->CmdLists[i];

            for (int j = 0; j < list->CmdBuffer.Size; j++) {
                const ImDrawCmd& command = list->CmdBuffer[j];

                if (command.UserCallback != nullptr) {
                    command.UserCallback(list, &command);
                    continue;
                }

                ImVec2 clipMin { (command.ClipRect.x - clipOff.x) * clipScale.x, (command.ClipRect.y - clipOff.y) * clipScale.y };
                ImVec2 clipMax { (command.ClipRect.z - clipOff.x) * clipScale.x, (command.ClipRect.w - clipOff.y) * clipScale.y };

                clipMin.x = std::max(clipMin.x, 0.0f);
                clipMin.y = std::max(clipMin.y, 0.0f);

                clipMax.x = std::min(clipMax.x, framebufferWidth);
                clipMax.y = std::min(clipMax.y, framebufferHeight);

                if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y) {
                    continue;
                }

                if (command.TextureId != nullptr) {
                    commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, backendData->pipeline,
                        1, &reinterpret_cast<const VkDescriptorSet&>(command.TextureId));
                }

                commandBuffer.setScissor({
                    .offset = { 
                        .x = static_cast<int32_t>(clipMin.x),
                        .y = static_cast<int32_t>(clipMin.y)
                    },
                    .extent = {
                        .width = static_cast<uint32_t>(clipMax.x - clipMin.x),
                        .height = static_cast<uint32_t>(clipMax.y - clipMin.y) 
                    }
                });
                commandBuffer.drawIndexed(command.ElemCount, 1U, indexOffset + command.IdxOffset, vertexOffset + command.VtxOffset);

                // We must restore default descriptor set
                if (command.TextureId != nullptr) {
                    commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, backendData->pipeline,
                        1, &backendData->descriptorSet->set());
                }
            }

            vertexOffset += list->VtxBuffer.Size;
            indexOffset += list->IdxBuffer.Size;
        }

        commandBuffer.endRenderPass();
    }

    void ImGuiSystem::initializeImGui()
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;

        VkExtent2D windowSize = applicationWindow->windowSize();
        io.DisplaySize = ImVec2 { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };

        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(io.BackendPlatformUserData = new BackendPlatformData {});
        backendData->gameSystem = std::make_unique<editor::MainSystem>(device, loopHandler);

        backendData->cursors[ImGuiMouseCursor_Arrow] =      coffee::Cursor::create(coffee::CursorType::Arrow);
        backendData->cursors[ImGuiMouseCursor_TextInput] =  coffee::Cursor::create(coffee::CursorType::TextInput);
        backendData->cursors[ImGuiMouseCursor_Hand] =       coffee::Cursor::create(coffee::CursorType::Hand);
        backendData->cursors[ImGuiMouseCursor_ResizeEW] =   coffee::Cursor::create(coffee::CursorType::ResizeEW);
        backendData->cursors[ImGuiMouseCursor_ResizeNS] =   coffee::Cursor::create(coffee::CursorType::ResizeNS);
        backendData->cursors[ImGuiMouseCursor_ResizeNWSE] = coffee::Cursor::create(coffee::CursorType::ResizeNWSE);
        backendData->cursors[ImGuiMouseCursor_ResizeNESW] = coffee::Cursor::create(coffee::CursorType::ResizeNESW);
        backendData->cursors[ImGuiMouseCursor_ResizeAll] =  coffee::Cursor::create(coffee::CursorType::ResizeAll);
        backendData->cursors[ImGuiMouseCursor_NotAllowed] = coffee::Cursor::create(coffee::CursorType::NotAllowed);

        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        updateMonitors();

        coffee::Monitor::monitorConnectedEvent += [](const coffee::MonitorPtr&) {
            static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors = true;
        };

        coffee::Monitor::monitorDisconnectedEvent += [](const coffee::MonitorPtr&) {
            static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors = true;
        };

        // Platform callbacks
        platformIO.Platform_CreateWindow = [](ImGuiViewport* viewport) {
            BackendPlatformData* backendPlatformData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
            BackendRendererData* backendRendererData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

            ViewportData* viewportData = new ViewportData {
                .windowHandle = coffee::Window::create(backendRendererData->device, {
                    .extent = { .width = static_cast<uint32_t>(viewport->Size.x), .height = static_cast<uint32_t>(viewport->Size.y) },
                    .presentMode = presentMode,
                    .hiddenOnStart = true
                    // BUG: No borderless here because if window is collapsed it will crash engine with 0 height size
                }).release()  // We must explicitly delete this
            };

            viewport->PlatformUserData = viewportData;

            coffee::Window* window = viewportData->windowHandle;

            window->userData = viewport->ID;
            window->setWindowPosition({ static_cast<int32_t>(viewport->Pos.x), static_cast<int32_t>(viewport->Pos.y) });

            window->windowFocusEvent += focusCallback;
            window->windowEnterEvent += enterCallback;
            window->mouseClickEvent += mouseClickCallback;
            window->mouseMoveEvent += mousePositionCallback;
            window->mouseWheelEvent += mouseWheelCallback;
            window->keyEvent += keyCallback;
            window->charEvent += charCallback;

            window->windowPositionEvent += [](const coffee::Window& window, const coffee::WindowPositionEvent& e) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window.userData))) {
                    if (ImGui::GetFrameCount() <= static_cast<ViewportData*>(viewport->PlatformUserData)->ignoreWindowPositionFrameCount + 1) {
                        return;
                    }

                    viewport->PlatformRequestMove = true;
                }
            };

            window->windowResizeEvent += [](const coffee::Window& window, const coffee::ResizeEvent& e) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window.userData))) {
                    BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
                    ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
                    RendererData* rendererData = static_cast<RendererData*>(viewport->RendererUserData);

                    const auto& presentImages = viewportData->windowHandle->presentImages();
                    rendererData->swapChainViews.resize(presentImages.size());
                    rendererData->framebuffers.resize(presentImages.size());

                    for (size_t i = 0; i < presentImages.size(); i++) {
                        rendererData->swapChainViews[i] = coffee::ImageView::create(presentImages[i], {
                            .viewType = VK_IMAGE_VIEW_TYPE_2D,
                            .format = backendData->device->surfaceFormat().format
                        });

                        rendererData->framebuffers[i] = coffee::Framebuffer::create(backendData->device, backendData->renderPass, {
                            .extent = viewportData->windowHandle->framebufferSize(),
                            .colorViews = { rendererData->swapChainViews[i] }
                        });
                    }

                    if (ImGui::GetFrameCount() <= static_cast<ViewportData*>(viewport->PlatformUserData)->ignoreWindowSizeFrameCount + 1) {
                        return;
                    }

                    viewport->PlatformRequestResize = true;
                }
            };

            window->windowCloseEvent += [](const coffee::Window& window) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window.userData))) {
                    viewport->PlatformRequestClose = true;
                }
            };

            if ((viewport->Flags & ImGuiViewportFlags_NoDecoration) != 0) {
                window->makeBorderless();
            }

            window->showWindow();
        };

        platformIO.Platform_DestroyWindow = [](ImGuiViewport* viewport) {
            if (ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData)) {
                BackendPlatformData* backendData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
                RendererData* rendererData = static_cast<RendererData*>(viewport->RendererUserData);

                for (int32_t i = 0; i < IM_ARRAYSIZE(backendData->keyOwnerWindows); i++) {
                    if (backendData->keyOwnerWindows[i] == viewportData->windowHandle) {
                        keyCallback(*viewportData->windowHandle, { coffee::State::Release, static_cast<coffee::Keys>(i), 0, 0 });
                    }
                }

                for (const auto& buffer : rendererData->vertexBuffers) {
                    if (buffer != nullptr) {
                        buffer->unmap();
                    }
                }

                for (const auto& buffer : rendererData->indexBuffers) {
                    if (buffer != nullptr) {
                        buffer->unmap();
                    }
                }

                // If user destroys window while cursor is captured
                if (viewportData->windowHandle == backendData->fullControlWindowPtr) {
                    backendData->fullControlWindowPtr = nullptr;
                    backendData->gameSystem->unbindWindow();
                }

                // We released this handle before, so we must explicitly delete it
                if (viewport != ImGui::GetMainViewport()) {
                    delete viewportData->windowHandle;
                }

                delete viewportData;
                viewport->PlatformUserData = nullptr;

                delete rendererData;
                viewport->RendererUserData = nullptr;
            }
        };

        platformIO.Platform_ShowWindow = [](ImGuiViewport* viewport) {
            static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->showWindow();
        };

        platformIO.Platform_SetWindowPos = [](ImGuiViewport* viewport, ImVec2 position) {
            ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
            viewportData->ignoreWindowPositionFrameCount = ImGui::GetFrameCount();
            viewportData->windowHandle->setWindowPosition({ static_cast<int32_t>(position.x), static_cast<int32_t>(position.y) });
        };

        platformIO.Platform_GetWindowPos = [](ImGuiViewport* viewport) -> ImVec2 {
            coffee::Window* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;
            VkOffset2D windowPosition = window->windowPosition();
            return { static_cast<float>(windowPosition.x), static_cast<float>(windowPosition.y) };
        };

        platformIO.Platform_SetWindowSize = [](ImGuiViewport* viewport, ImVec2 size) {
            ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
            viewportData->ignoreWindowSizeFrameCount = ImGui::GetFrameCount();
            viewportData->windowHandle->setWindowSize({ static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y) });
        };

        platformIO.Platform_GetWindowSize = [](ImGuiViewport* viewport) -> ImVec2 {
            coffee::Window* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;
            VkExtent2D windowSize = window->windowSize();
            return { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };
        };

        platformIO.Platform_SetWindowFocus = [](ImGuiViewport* viewport) {
            static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->focusWindow();
        };

        platformIO.Platform_GetWindowFocus = [](ImGuiViewport* viewport) -> bool {
            return static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->isFocused();
        };

        platformIO.Platform_GetWindowMinimized = [](ImGuiViewport* viewport) -> bool {
            bool result = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->isIconified();
            return result;
        };

        platformIO.Platform_SetWindowTitle = [](ImGuiViewport* viewport, const char* str) {
            static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->setWindowTitle(str);
        };

        platformIO.Platform_RenderWindow = [](ImGuiViewport*, void*) { /* Already handled by Renderer_RenderWindow */ };
        platformIO.Platform_SwapBuffers = [](ImGuiViewport*, void*) { /* Already handled by Renderer_SwapBuffers */ };

        // Renderer callbacks
        platformIO.Renderer_CreateWindow = [](ImGuiViewport* viewport) {
            BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            RendererData* rendererData = static_cast<RendererData*>(viewport->RendererUserData = new RendererData {});

            coffee::Window* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;
            const auto& presentImages = window->presentImages();
            rendererData->swapChainViews.resize(presentImages.size());
            rendererData->framebuffers.resize(presentImages.size());

            for (size_t i = 0; i < presentImages.size(); i++) {
                rendererData->swapChainViews[i] = coffee::ImageView::create(presentImages[i], {
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = backendData->device->surfaceFormat().format
                });

                rendererData->framebuffers[i] = coffee::Framebuffer::create(backendData->device, backendData->renderPass, {
                    .extent = window->framebufferSize(),
                    .colorViews = { rendererData->swapChainViews[i] } 
                });
            }

            rendererData->vertexBuffers.resize(presentImages.size());
            rendererData->indexBuffers.resize(presentImages.size());
        };

        platformIO.Renderer_DestroyWindow = [](ImGuiViewport*) {
            // Already handled by Platform_DestroyWindow
            // Must be here as placeholder, otherwise viewports won't be able to merge
        };

        platformIO.Renderer_RenderWindow = [](ImGuiViewport* viewport, void*) {
            BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);

            if (viewportData->windowHandle->acquireNextImage()) {
                coffee::CommandBuffer commandBuffer = coffee::CommandBuffer::createGraphics(backendData->device);
                ImGuiSystem::render(viewport, commandBuffer);
                viewportData->windowHandle->sendCommandBuffer(std::move(commandBuffer));
            }
        };

        applicationWindow->windowFocusEvent += focusCallback;
        applicationWindow->windowEnterEvent += enterCallback;
        applicationWindow->mouseClickEvent += mouseClickCallback;
        applicationWindow->mouseMoveEvent += mousePositionCallback;
        applicationWindow->mouseWheelEvent += mouseWheelCallback;
        applicationWindow->keyEvent += keyCallback;
        applicationWindow->charEvent += charCallback;

        applicationWindow->windowResizeEvent += [](const coffee::Window& window, const coffee::ResizeEvent& e) {
            BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            ViewportData* viewportData = static_cast<ViewportData*>(ImGui::GetMainViewport()->PlatformUserData);
            RendererData* rendererData = static_cast<RendererData*>(ImGui::GetMainViewport()->RendererUserData);

            const auto& presentImages = viewportData->windowHandle->presentImages();
            rendererData->swapChainViews.resize(presentImages.size());
            rendererData->framebuffers.resize(presentImages.size());

            for (size_t i = 0; i < presentImages.size(); i++) {
                rendererData->swapChainViews[i] = coffee::ImageView::create(presentImages[i], {
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = backendData->device->surfaceFormat().format 
                });

                rendererData->framebuffers[i] = coffee::Framebuffer::create(backendData->device, backendData->renderPass, {
                    .extent = viewportData->windowHandle->framebufferSize(), 
                    .colorViews = { rendererData->swapChainViews[i] } 
                });
            }
        };

        applicationWindow->userData = 0;

        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        mainViewport->PlatformUserData = new ViewportData { .windowHandle = applicationWindow.get() };
    }

    void ImGuiSystem::initializeBackend()
    {
        BackendRendererData* backendData =
            static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData = new BackendRendererData {});
        RendererData* rendererData = static_cast<RendererData*>(ImGui::GetMainViewport()->RendererUserData = new RendererData {});

        backendData->device = device;

        createFonts();
        createDescriptors();
        createRenderPass();
        createPipeline();
        createQueryPool();

        const VkPhysicalDeviceMemoryProperties* properties = device->memoryProperties();

        for (uint32_t i = 0; i < properties->memoryTypeCount; i++) {
            const VkMemoryType& memoryType = properties->memoryTypes[i];

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

        const auto& presentImages = applicationWindow->presentImages();
        rendererData->swapChainViews.resize(presentImages.size());
        rendererData->framebuffers.resize(presentImages.size());

        for (size_t i = 0; i < presentImages.size(); i++) {
            rendererData->swapChainViews[i] = coffee::ImageView::create(presentImages[i], {
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = backendData->device->surfaceFormat().format 
            });

            rendererData->framebuffers[i] = coffee::Framebuffer::create(backendData->device, backendData->renderPass, {
                .extent = applicationWindow->framebufferSize(), 
                .colorViews = { rendererData->swapChainViews[i] }
            });
        }

        rendererData->vertexBuffers.resize(presentImages.size());
        rendererData->indexBuffers.resize(presentImages.size());
    }

    void ImGuiSystem::createFonts()
    {
        ImGuiIO& io = ImGui::GetIO();
        uint8_t* fontPixels = nullptr;
        int width {}, height {};

        ImFontConfig fontConfiguration {};
        fontConfiguration.SizePixels = 16.0f;
        fontConfiguration.OversampleH = 1;
        fontConfiguration.OversampleV = 1;
        fontConfiguration.PixelSnapH = true;
        fontConfiguration.GlyphOffset.y = 1.0f * std::floor(fontConfiguration.SizePixels / 13.0f);

        io.Fonts->AddFontFromMemoryCompressedBase85TTF(
            FiraFontInBase85,
            fontConfiguration.SizePixels,
            &fontConfiguration,
            io.Fonts->GetGlyphRangesCyrillic()
        );

        io.Fonts->Build();
        io.Fonts->GetTexDataAsAlpha8(&fontPixels, &width, &height);

        BackendRendererData* backendData = static_cast<BackendRendererData*>(io.BackendRendererUserData);

        backendData->fontsSampler = coffee::Sampler::create(backendData->device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .anisotropyEnable = true,
            .maxAnisotropy = 1U,
            .minLod = -1000.0f,
            .maxLod = 1000.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
        });

        backendData->fonts = coffee::Image::create(backendData->device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R8_UNORM,
            .extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .priority = 1.0f
        });

        coffee::BufferPtr stagingBuffer = coffee::Buffer::create(backendData->device, {
            .instanceSize = static_cast<uint32_t>(width * height),
            .instanceCount = 1U,
            .usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
        });

        stagingBuffer->map();
        {
            std::memcpy(stagingBuffer->memory(), fontPixels, static_cast<size_t>(width * height));
            stagingBuffer->flush();
        }
        stagingBuffer->unmap();

        coffee::SingleTime::copyBufferToImage(device, backendData->fonts, stagingBuffer);

        backendData->fontsView = coffee::ImageView::create(backendData->fonts, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8_UNORM,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_ONE,
                .g = VK_COMPONENT_SWIZZLE_ONE,
                .b = VK_COMPONENT_SWIZZLE_ONE,
                .a = VK_COMPONENT_SWIZZLE_R
            }
        });
    }

    void ImGuiSystem::createDescriptors()
    {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        const std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {
            { 0, { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .shaderStages = VK_SHADER_STAGE_FRAGMENT_BIT } }
        };

        backendData->layout = coffee::DescriptorLayout::create(device, bindings);
        backendData->descriptorSet = coffee::DescriptorSet::create(device, coffee::DescriptorWriter(backendData->layout)
            .addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, backendData->fontsView, backendData->fontsSampler));
    }

    void ImGuiSystem::createRenderPass()
    {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        backendData->renderPass = coffee::RenderPass::create(device, {
            .colorAttachments = { coffee::AttachmentConfiguration {
                .format = device->surfaceFormat().format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            }}
        });
    }

    void ImGuiSystem::createPipeline()
    {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        backendData->pipeline = coffee::Pipeline::create(device, backendData->renderPass, {
            .shaders = coffee::utils::moveList<coffee::ShaderPtr, std::vector<coffee::ShaderPtr>>({
                coffee::ShaderModule::create(device, imguiVertexShader, VK_SHADER_STAGE_VERTEX_BIT),
                coffee::ShaderModule::create(device, imguiFragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
            }),
            .layouts = {
                backendData->layout
            },
            .inputBindings = { coffee::InputBinding {
                .binding = 0U,
                .stride = sizeof(ImDrawVert),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = { 
                    coffee::InputElement { .location = 0U, .format = VK_FORMAT_R32G32_SFLOAT, .offset = offsetof(ImDrawVert, pos) },
                    coffee::InputElement { .location = 1U, .format = VK_FORMAT_R32G32_SFLOAT, .offset = offsetof(ImDrawVert, uv) },
                    coffee::InputElement { .location = 2U, .format = VK_FORMAT_R8G8B8A8_UNORM, .offset = offsetof(ImDrawVert, col) } 
                }
            }},
            .constantRanges = { coffee::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT,
                .size = static_cast<uint32_t>(sizeof(ImGuiPushConstant)) 
            }},
            .rasterizationInfo = {
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE
            },
            .colorBlendAttachment = { 
                .blendEnable = true,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
            },
            .depthStencilInfo = {
                .depthTestEnable = false,
                .depthWriteEnable = false,
                .depthCompareOp = VK_COMPARE_OP_NEVER
            }
        });
    }

    void ImGuiSystem::focusCallback(const coffee::Window& window, const coffee::WindowFocusEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(!e.lost);
    }

    void ImGuiSystem::enterCallback(const coffee::Window& window, const coffee::WindowEnterEvent& e)
    {
        if (window.cursorState() == coffee::CursorState::Disabled) {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(io.BackendPlatformUserData);

        if (e.entered) {
            backendData->windowPtr = &window;

            io.AddMousePosEvent(backendData->lastMousePos.x, backendData->lastMousePos.y);
        }
        else if (backendData->windowPtr == &window) {
            backendData->windowPtr = nullptr;
            backendData->lastMousePos = io.MousePos;

            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        }
    }

    void ImGuiSystem::mouseClickCallback(const coffee::Window& window, const coffee::MouseClickEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl, e.control);
        io.AddKeyEvent(ImGuiMod_Shift, e.shift);
        io.AddKeyEvent(ImGuiMod_Alt, e.alt);
        io.AddKeyEvent(ImGuiMod_Super, e.super);

        uint8_t button = static_cast<uint8_t>(e.button);
        if (button >= 0 && button < ImGuiMouseButton_COUNT) {
            io.AddMouseButtonEvent(button, e.state == coffee::State::Press);
        }
    }

    void ImGuiSystem::mousePositionCallback(const coffee::Window& window, const coffee::MouseMoveEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(io.BackendPlatformUserData);

        float xPosition = e.x;
        float yPosition = e.y;

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            VkOffset2D windowPosition = window.windowPosition();
            xPosition += windowPosition.x;
            yPosition += windowPosition.y;
        }

        io.AddMousePosEvent(xPosition, yPosition);
        backendData->lastMousePos = { xPosition, yPosition };
    }

    void ImGuiSystem::mouseWheelCallback(const coffee::Window& window, const coffee::MouseWheelEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(e.x, e.y);
    }

    void ImGuiSystem::keyCallback(const coffee::Window& window, const coffee::KeyEvent& e)
    {
        if (e.state != coffee::State::Press && e.state != coffee::State::Release) {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl, e.control);
        io.AddKeyEvent(ImGuiMod_Shift, e.shift);
        io.AddKeyEvent(ImGuiMod_Alt, e.alt);
        io.AddKeyEvent(ImGuiMod_Super, e.super);

        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        backendData->keyOwnerWindows[static_cast<uint32_t>(e.key)] = (e.state == coffee::State::Press) ? &window : nullptr;

        ImGuiKey key = keyToImGui(e.key);
        io.AddKeyEvent(key, e.state == coffee::State::Press);
        io.SetKeyEventNativeData(key, static_cast<int>(e.key), static_cast<int>(e.scancode));
    }

    void ImGuiSystem::charCallback(const coffee::Window& window, char32_t ch)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(ch);
    }

    void ImGuiSystem::updateMonitors()
    {
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        platformIO.Monitors.resize(0);

        for (const auto& monitor : coffee::Monitor::monitors()) {
            ImGuiPlatformMonitor imGuiMonitor {};

            VkExtent2D position = monitor->position();
            imGuiMonitor.MainPos = imGuiMonitor.WorkPos =
                ImVec2 { static_cast<float>(position.width), static_cast<float>(position.height) };

            coffee::VideoMode videoMode = monitor->currentVideoMode();
            imGuiMonitor.MainSize = imGuiMonitor.WorkSize =
                ImVec2 { static_cast<float>(videoMode.width), static_cast<float>(videoMode.height) };

            VkRect2D workArea = monitor->workArea();
            if (workArea.extent.width > 0 && workArea.extent.height > 0) {
                imGuiMonitor.WorkPos = ImVec2 { static_cast<float>(workArea.offset.x), static_cast<float>(workArea.offset.y) };
                imGuiMonitor.WorkSize = ImVec2 { static_cast<float>(workArea.extent.width), static_cast<float>(workArea.extent.height) };
            }

            coffee::Float2D scale = monitor->contentScale();
            imGuiMonitor.DpiScale = scale.x;

            platformIO.Monitors.push_back(imGuiMonitor);
        }
    }

    void ImGuiSystem::updateMouse(const coffee::WindowPtr& applicationWindow)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(io.BackendPlatformUserData);

        if (backendData->fullControlWindowPtr != nullptr) {
            if (!backendData->fullControlWindowPtr->isButtonPressed(coffee::Keys::Escape)) {
                io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
                return;
            }

            backendData->fullControlWindowPtr = nullptr;
            backendData->gameSystem->unbindWindow();
        }

        ImGuiID mouseViewportID = 0;
        const ImVec2 previousMousePosition = io.MousePos;

        for (int32_t i = 0; i < platformIO.Viewports.Size; i++) {
            ImGuiViewport* viewport = platformIO.Viewports[i];
            coffee::Window* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;

            if (window->isFocused()) {
                if (io.WantSetMousePos) {
                    window->setMousePosition({ previousMousePosition.x - viewport->Pos.x, previousMousePosition.y - viewport->Pos.y });
                }

                if (backendData->windowPtr == nullptr) {
                    coffee::Float2D mousePosition = window->mousePosition();

                    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                        VkOffset2D windowPosition = window->windowPosition();
                        mousePosition.x += windowPosition.x;
                        mousePosition.y += windowPosition.y;
                    }

                    backendData->lastMousePos = ImVec2 { mousePosition.x, mousePosition.y };
                    io.AddMousePosEvent(mousePosition.x, mousePosition.y);
                }
            }

            bool windowNoInput = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;

            windowNoInput ? window->enablePassthrough() : window->disablePassthrough();

            if (window->isFocused() && !windowNoInput) {
                mouseViewportID = viewport->ID;
            }
        }

        if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport) {
            io.AddMouseViewportEvent(mouseViewportID);
        }
    }

    void ImGuiSystem::updateCursor(const coffee::WindowPtr& applicationWindow)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (static_cast<BackendPlatformData*>(io.BackendPlatformUserData)->fullControlWindowPtr != nullptr) {
            return;
        }

        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

        for (int32_t i = 0; i < platformIO.Viewports.Size; i++) {
            ImGuiViewport* viewport = platformIO.Viewports[i];
            coffee::Window* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;

            if (cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
                window->hideCursor();
                continue;
            }

            window->setCursor(backendData->cursors[cursor]);
            window->showCursor();
        }
    }

    void ImGuiSystem::createQueryPool()
    {
        VkQueryPoolCreateInfo createInfo { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
        createInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        createInfo.queryCount = timestampQueryCount;

        if (vkCreateQueryPool(device->logicalDevice(), &createInfo, nullptr, &timestampQueryPool) != VK_SUCCESS) {
            timestampQueryPool = VK_NULL_HANDLE;
        }
    }

    void ImGuiSystem::destroyQueryPool()
    {
        if (timestampQueryPool != VK_NULL_HANDLE) {
            vkDestroyQueryPool(device->logicalDevice(), timestampQueryPool, nullptr);
        }
    }

    void ImGuiSystem::resetQueryPool(const coffee::CommandBuffer& commandBuffer)
    {
        if (timestampQueryPool != VK_NULL_HANDLE) {
            vkCmdResetQueryPool(commandBuffer, timestampQueryPool, 0, timestampQueryCount);
        }
    }

    void ImGuiSystem::beginTimestamp(const coffee::CommandBuffer& commandBuffer, uint32_t index)
    {
        if (timestampQueryPool != VK_NULL_HANDLE) {
            vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, timestampQueryPool, 2 * index);
        }
    }

    void ImGuiSystem::endTimestamp(const coffee::CommandBuffer& commandBuffer, uint32_t index)
    {
        if (timestampQueryPool != VK_NULL_HANDLE) {
            vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, timestampQueryPool, 2 * index + 1);
        }
    }

    float ImGuiSystem::getTimestampResult(uint32_t index)
    {
        if (timestampQueryPool == VK_NULL_HANDLE) {
            return 0;
        }

        uint64_t results[2] {};
        VkResult result = vkGetQueryPoolResults(
            device->logicalDevice(),
            timestampQueryPool,
            2 * index,
            2,
            sizeof(results),
            results,
            sizeof(uint64_t),
            VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT
        );

        if (result == VK_SUCCESS) {
            return (results[1] - results[0]) * device->properties().limits.timestampPeriod;
        }

        return 0;
    }

    bool ImGuiSystem::isAnyWindowActive()
    {
        for (ImGuiViewportP* viewport : ImGui::GetCurrentContext()->Viewports) {
            if (ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData)) {
                if (!viewportData->windowHandle->isIconified()) {
                    return true;
                }
            }
        }

        return false;
    }

    void ImGuiSystem::createProject() {}

    void ImGuiSystem::loadProject()
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

    void ImGuiSystem::loadAssetDirectory()
    {
        assetBrowser.content.clear();

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
                    case coffee::utils::fnv1a::digest(".cfz"):
                        newEntry.type = DirectoryObject::Type::Texture;
                        break;
                    case coffee::utils::fnv1a::digest(".cfa"):
                        newEntry.type = DirectoryObject::Type::Mesh;
                        break;
                    default:
                        break;
                }
            }
        }

        assetBrowser.updateRequired = false;
    }

    void ImGuiSystem::saveProject() {}

    void ImGuiSystem::removeChilds(entt::entity childEntity)
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

    void ImGuiSystem::drawHierarchyTree(entt::entity entity)
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
                hierachyComponent.tag.data(),
                hierachyComponent.tag.data() + hierachyComponent.tag.size(),
                flags
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

                    sceneHierarchy.components.emplace<PointLight>(child).transform =
                        static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->gameSystem->viewerObject;
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

                    // TODO: sceneHierarchy.components.emplace<SpotLight>();
                }

                if (ImGui::MenuItem("New Directional Light")) {
                    const auto child = sceneHierarchy.components.create();

                    entt::entity previousChild = hierachyComponent.firstChild;
                    hierachyComponent.firstChild = child;

                    auto& childHierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(child);
                    childHierarchyComponent.parent = entity;
                    childHierarchyComponent.next = previousChild;
                    childHierarchyComponent.tag = "Directional Light";

                    if (auto* previousChildHierarchyComponent = sceneHierarchy.components.try_get<HierarchyComponent>(previousChild)) {
                        previousChildHierarchyComponent->previous = child;
                    }

                    // TODO: sceneHierarchy.components.emplace<DirectionalLight>();
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

    void ImGuiSystem::prepareImGui()
    {
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);

        ImGui::NewFrame();

        // Allow free movement of sub-windows inside main viewport
        ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        {
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu(localizeName(LocaleName::File), !projectInformation.dialog.IsOpened())) {
                    if (ImGui::MenuItem(localizeName(LocaleName::NewProject))) {
                        projectInformation.dialog.OpenDialog("CreateProjectDialog", "Select dictionary", nullptr, ".");
                        projectInformation.loaded = true;
                    }

                    if (ImGui::MenuItem(localizeName(LocaleName::OpenProject))) {
                        projectInformation.dialog.OpenDialog("LoadProjectDialog", "Select .cfpj file", ".cfpj", ".");
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
                    ImGui::MenuItem(localizeName(LocaleName::HierarchyComponent), nullptr, &sceneHierarchy.active, projectInformation.loaded);
                    ImGui::MenuItem(localizeName(LocaleName::SceneViewport), nullptr, &sceneViewport.active, projectInformation.loaded);
                    ImGui::MenuItem(localizeName(LocaleName::AssetBrowser), nullptr, &assetBrowser.active, projectInformation.loaded);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu(localizeName(LocaleName::Language))) {
                    if (ImGui::MenuItem(reinterpret_cast<const char*>(u8"English"), nullptr, currentLocale == CurrentLocale::EN)) {
                        currentLocale = CurrentLocale::EN;
                        IGFDLocale::currentLocale = CurrentLocale::EN;
                    }

                    if (ImGui::MenuItem(reinterpret_cast<const char*>(u8"�������"), nullptr, currentLocale == CurrentLocale::RU)) {
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
                std::string sceneHierarchyName = std::format("{}###HierarchyViewport", localizeName(LocaleName::HierarchyComponent));
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
                        sceneHierarchy.components.emplace<PointLight>(entity).transform = backendData->gameSystem->viewerObject;
                    }

                    if (ImGui::MenuItem("New Spot Light")) {
                        const auto entity = sceneHierarchy.components.create();

                        auto& hierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(entity);
                        hierarchyComponent.parent = entt::null;
                        hierarchyComponent.tag = "Spot Light";

                        sceneHierarchy.components.emplace<EntityRoot>(entity);

                        // TODO: sceneHierarchy.components.emplace<SpotLight>();
                    }

                    if (ImGui::MenuItem("New Directional Light")) {
                        const auto entity = sceneHierarchy.components.create();

                        auto& hierarchyComponent = sceneHierarchy.components.emplace<HierarchyComponent>(entity);
                        hierarchyComponent.parent = entt::null;
                        hierarchyComponent.tag = "Directional Light";

                        sceneHierarchy.components.emplace<EntityRoot>(entity);

                        // TODO: sceneHierarchy.components.emplace<DirectionalLight>();
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
                backendData->gameSystem->outputAspect.store(
                    sceneViewport.keepAspectRatio ? 16.0f / 9.0f : contentRegion.x / contentRegion.y,
                    std::memory_order_relaxed
                );

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });

                if (ImGui::ImageButton(
                        "VP1: Take control",
                        framebufferImage->set(),
                        contentRegion,
                        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_PressedOnClick
                )) {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        // TODO: ?
                    }
                    else {
                        coffee::Window* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;

                        backendData->fullControlWindowPtr = window;
                        backendData->gameSystem->bindWindow(window);

                        window->disableCursor();
                    }
                }

                ImGui::PopStyleVar();

                constexpr float timestampToMillseconds = 1000000.0f;
                constexpr float bytesToMegabytes = 1000000.0f;

                sceneViewport.averageFrameTime[sceneViewport.statisticIndex] = 1000.0f * io.DeltaTime;
                sceneViewport.averageFPS[sceneViewport.statisticIndex] = io.Framerate;
                sceneViewport.averageDepthPass[sceneViewport.statisticIndex] = getTimestampResult(0) / timestampToMillseconds;
                sceneViewport.averageRendering[sceneViewport.statisticIndex] = getTimestampResult(1) / timestampToMillseconds;

                if (sceneViewport.outputFramerateAndFPS) {
                    ImVec2 textPosition = { currentCursorPosition.x + 10, currentCursorPosition.y + 10 };

                    std::string framerateAndFPS = std::format(
                        "{:.3f} ms/frame ({:.1f} FPS)",
                        calculateAverage(sceneViewport.averageFrameTime),
                        calculateAverage(sceneViewport.averageFPS)
                    );
                    ImGui::GetWindowDrawList()->AddText(
                        textPosition,
                        IM_COL32(0, 255, 0, 255),
                        framerateAndFPS.data(),
                        framerateAndFPS.data() + framerateAndFPS.size()
                    );

                    textPosition.y += 15;
                    std::string cpuTime = std::format("CPU: {:.6f} ms", calculateAverage(sceneViewport.averageCPUTime));
                    ImGui::GetWindowDrawList()->AddText(textPosition, IM_COL32(0, 255, 0, 255), cpuTime.data(), cpuTime.data() + cpuTime.size());

                    textPosition.y += 15;
                    std::string depthPass = std::format("Depth pass: {:.6f} ms", calculateAverage(sceneViewport.averageDepthPass));
                    ImGui::GetWindowDrawList()->AddText(textPosition, IM_COL32(0, 255, 0, 255), depthPass.data(), depthPass.data() + depthPass.size());

                    textPosition.y += 15;
                    std::string rendering = std::format("Rendering: {:.6f} ms", calculateAverage(sceneViewport.averageRendering));
                    ImGui::GetWindowDrawList()->AddText(textPosition, IM_COL32(0, 255, 0, 255), rendering.data(), rendering.data() + rendering.size());

                    textPosition.y += 15;
                    std::array<VmaBudget, VK_MAX_MEMORY_HEAPS> budgets = device->heapBudgets();

                    if (sceneViewport.deviceHeapIndex != SceneViewport::invalidHeapIndex) {
                        VmaBudget& budget = budgets[sceneViewport.deviceHeapIndex];
                        float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.f;

                        textPosition.y += 15;
                        std::string overallUsage = std::format(
                            "VRAM: {:.2f}/{:.2f} mb ({:.2f}%)",
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

                    if (sceneViewport.hostHeapIndex != SceneViewport::invalidHeapIndex) {
                        VmaBudget& budget = budgets[sceneViewport.hostHeapIndex];
                        float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.f;

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

                    if (sceneViewport.sharedHeapIndex != SceneViewport::invalidHeapIndex) {
                        VmaBudget& budget = budgets[sceneViewport.sharedHeapIndex];
                        float percentage = static_cast<float>(budget.usage) / static_cast<float>(budget.budget) * 100.f;

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

                for (size_t i = 0; i < assetBrowser.content.size(); i++) {
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

} // namespace editor