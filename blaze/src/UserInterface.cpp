#include "UserInterface.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Blaze.hpp"
#include "Mesh.hpp"
#include "Input.hpp"
#include "VulkanMaterial.hpp"

#include <imgui.h>
#include <glm/glm.hpp>
#include <imgui_internal.h>
#include <vulkan/vulkan.hpp>
#include <VulkanGraphicsBuffer.hpp>

UserInterface::UserInterface(glm::u32 frameCount){
    IMGUI_CHECKVERSION();
    _ctx = ImGui::CreateContext();

    _ctx->IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    _ctx->IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    _ctx->IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    _ctx->IO.BackendPlatformName = "imgui_impl_glfw";
    _ctx->IO.BackendRendererName = "imgui_impl_vulkan";

    // Keyboard mapping. ImGui will use those indices to peek into the _ctx->IO.KeysDown[] array.
    _ctx->IO.KeyMap[ImGuiKey_Tab] = int(KeyCode::eTab);
    _ctx->IO.KeyMap[ImGuiKey_LeftArrow] = int(KeyCode::eLeftArrow);
    _ctx->IO.KeyMap[ImGuiKey_RightArrow] = int(KeyCode::eRightArrow);
    _ctx->IO.KeyMap[ImGuiKey_UpArrow] = int(KeyCode::eUpArrow);
    _ctx->IO.KeyMap[ImGuiKey_DownArrow] = int(KeyCode::eDownArrow);
    _ctx->IO.KeyMap[ImGuiKey_PageUp] = int(KeyCode::ePageUp);
    _ctx->IO.KeyMap[ImGuiKey_PageDown] = int(KeyCode::ePageDown);
    _ctx->IO.KeyMap[ImGuiKey_Home] = int(KeyCode::eHome);
    _ctx->IO.KeyMap[ImGuiKey_End] = int(KeyCode::eEnd);
    _ctx->IO.KeyMap[ImGuiKey_Insert] = int(KeyCode::eInsert);
    _ctx->IO.KeyMap[ImGuiKey_Delete] = int(KeyCode::eDelete);
    _ctx->IO.KeyMap[ImGuiKey_Backspace] = int(KeyCode::eBackspace);
    _ctx->IO.KeyMap[ImGuiKey_Space] = int(KeyCode::eSpace);
    _ctx->IO.KeyMap[ImGuiKey_Enter] = int(KeyCode::eEnter);
    _ctx->IO.KeyMap[ImGuiKey_Escape] = int(KeyCode::eEscape);
    _ctx->IO.KeyMap[ImGuiKey_KeyPadEnter] = int(KeyCode::eKeyPadEnter);
    _ctx->IO.KeyMap[ImGuiKey_A] = int(KeyCode::eA);
    _ctx->IO.KeyMap[ImGuiKey_C] = int(KeyCode::eC);
    _ctx->IO.KeyMap[ImGuiKey_V] = int(KeyCode::eV);
    _ctx->IO.KeyMap[ImGuiKey_X] = int(KeyCode::eX);
    _ctx->IO.KeyMap[ImGuiKey_Y] = int(KeyCode::eY);
    _ctx->IO.KeyMap[ImGuiKey_Z] = int(KeyCode::eZ);

//    _ctx->IO.SetClipboardTextFn = SetClipboardText;
//    _ctx->IO.GetClipboardTextFn = GetClipboardText;
//    _ctx->IO.ClipboardUserData = BlazeContext::Get()->window();

//    _mouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    _mouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
//    _mouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
//    _mouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
//    _mouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
//#if GLFW_HAS_NEW_CURSORS
//    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
//#else
//    _mouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    _mouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    _mouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    _mouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//#endif

    _frameIndex = 0;
    _frameCount = frameCount;
    _frames.resize(_frameCount);

    ImGui::StyleColorsDark(&_ctx->Style);

    _ctx->Style.WindowRounding = 0.0f;
    _ctx->Style.ChildRounding = 0.0f;
    _ctx->Style.FrameRounding = 0.0f;
    _ctx->Style.GrabRounding = 0.0f;
    _ctx->Style.PopupRounding = 0.0f;
    _ctx->Style.ScrollbarRounding = 0.0f;

    _material = Material::LoadFromResources("blaze:materials/imgui.material");

    _createFontsTexture();
}

UserInterface::~UserInterface() {
    ImGui::DestroyContext(_ctx);
    //    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    //    {
    //        glfwDestroyCursor(_mouseCursors[cursor_n]);
    //        _mouseCursors[cursor_n] = NULL;
    //    }

    _frames.clear();
}

void UserInterface::setDisplaySize(const glm::ivec2& size) {
    _ctx->IO.DisplaySize = {
        static_cast<float>(size.x),
        static_cast<float>(size.y)
    };
}

void UserInterface::setDisplayScale(const glm::vec2& scale) {
    _ctx->IO.DisplayFramebufferScale = {
        scale.x,
        scale.y
    };
}

void UserInterface::setDeltaTime(float delta) {
    _ctx->IO.DeltaTime = delta;
}

void UserInterface::setMousePosition(const glm::vec2& pos) {
    _ctx->IO.MousePos = {
        pos.x,
        pos.y
    };
}

void UserInterface::setMousePressed(int button, bool flag) {
    _ctx->IO.MouseDown[button] = flag;
}

void UserInterface::setKeyPressed(int button, bool flag) {
    _ctx->IO.KeysDown[button] = flag;
}

void UserInterface::draw(CommandBuffer cmd) {
    const auto viewport = _ctx->Viewports[0];
    if (!viewport->DrawDataP.Valid) {
        return;
    }

    const auto drawData = std::addressof(viewport->DrawDataP);

    const auto displayPos = drawData->DisplayPos;
    const auto displaySize = drawData->DisplaySize;
    const auto displayScale = drawData->FramebufferScale;
    const auto displayRect = ImRect(displayPos, displayPos + displaySize);

    const auto fbSize = displaySize * displayScale;
    const auto fb_width = static_cast<int>(fbSize.x);
    const auto fb_height = static_cast<int>(fbSize.y);
    if (fb_width <= 0 || fb_height <= 0) {
        return;
    }

    auto& frame = _frames[_frameIndex];

    if (drawData->TotalVtxCount > 0) {
        frame.setVertexBufferParams(drawData->TotalVtxCount, sizeof(ImDrawVert));
        frame.setIndexBufferParams(drawData->TotalIdxCount, sizeof(ImDrawIdx));

        auto vtx_dst_offset = 0;
        auto idx_dst_offset = 0;
        for (int n = 0; n < drawData->CmdListsCount; n++) {
            auto cmd_list = drawData->CmdLists[n];
            frame.setVertexBufferData(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), vtx_dst_offset);
            frame.setIndexBufferData(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), idx_dst_offset);
            vtx_dst_offset += static_cast<int>(cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            idx_dst_offset += static_cast<int>(cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        }
    }

    _setupRenderState(drawData, cmd, &frame, fb_width, fb_height);

    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    for (int n = 0; n < drawData->CmdListsCount; n++) {
        const auto cmd_list = drawData->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            auto pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr) {
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState) {
                    _setupRenderState(drawData, cmd, &frame, fb_width, fb_height);
                } else {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
            } else {
                auto clipRect = ImRect(pcmd->ClipRect);
                if (!clipRect.Overlaps(displayRect)) {
                    continue;
                }
                clipRect.ClipWithFull(displayRect);

                const auto offset = (clipRect.Min - displayPos) * displayScale;
                const auto extent = clipRect.GetSize() * displayScale;

                const auto scissor = vk::Rect2D{
                    .offset = vk::Offset2D {
                        static_cast<int>(offset.x),
                        static_cast<int>(offset.y)
                    },
                    .extent = vk::Extent2D {
                        static_cast<uint32_t>(extent.x),
                        static_cast<uint32_t>(extent.y)
                    }
                };

//                    const auto texture = (vk::DescriptorSet)(VkDescriptorSettorSet)pcmd->GetTexID();
//                    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, material.getPipelineLayout(), 0, { texture }, {});

                (*cmd).setScissor(0, 1, &scissor);
                (*cmd).drawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    _frameIndex = (_frameIndex + 1) % _frameCount;
}

void UserInterface::begin() {
    ImGui::SetCurrentContext(_ctx);
    ImGui::NewFrame();
}

void UserInterface::end() {
    ImGui::Render();
    ImGui::SetCurrentContext(nullptr);
}

void UserInterface::_createFontsTexture() {
    uint8_t *pixels;
    int width, height;
    _ctx->IO.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    _fontTexture = Texture2D(glm::u32(width), glm::u32(height), vk::Format::eR8G8B8A8Unorm);
    _fontTexture.setPixels(std::span(reinterpret_cast<const glm::u8vec4 *>(pixels), width * height));
    _material.SetTexture(0, _fontTexture);
}

void UserInterface::_setupRenderState(ImDrawData* draw_data, CommandBuffer cmd, Mesh* rb, int fb_width, int fb_height) {
    auto vk_material = static_cast<VulkanMaterial*>(_material.GetNativeHandlePtr());

    (*cmd).bindPipeline(vk::PipelineBindPoint::eGraphics, vk_material->pipeline);
    (*cmd).bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        vk_material->pipelineLayout,
        0,
        vk_material->descriptorSets,
        vk_material->dynamicOffsets
    );

    if (draw_data->TotalVtxCount > 0) {
        auto vtx_ptr = static_cast<VulkanGraphicsBuffer*>(rb->getNativeVertexBufferPtr());
        auto idx_ptr = static_cast<VulkanGraphicsBuffer*>(rb->getNativeIndexBufferPtr());

        (*cmd).bindVertexBuffers(0, vtx_ptr->buffer, vk::DeviceSize{0});
        (*cmd).bindIndexBuffer(idx_ptr->buffer, 0, vk::IndexType::eUint16);
    }

    (*cmd).setViewport(0, vk::Viewport{0, 0, float(fb_width), float(fb_height), 0, 1});

    const auto transform = std::array {
        2.0f / draw_data->DisplaySize.x,
        2.0f / draw_data->DisplaySize.y,
        -1.0f - draw_data->DisplayPos.x * (2.0f / draw_data->DisplaySize.x),
        -1.0f - draw_data->DisplayPos.y * (2.0f / draw_data->DisplaySize.y)
    };

    (*cmd).pushConstants(
        vk_material->pipelineLayout,
        vk::ShaderStageFlagBits::eVertex,
        0,
        std::span(transform).size_bytes(),
        transform.data()
    );
}
void UserInterface::AddInputCharacter(char ch) {
    _ctx->IO.AddInputCharacter(ch);
}