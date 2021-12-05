#include "UserInterface.hpp"
#include "RenderPipeline.hpp"
#include "Resources.hpp"
#include "Timestamp.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Blaze.hpp"
#include "Mesh.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>

struct UserInterface::Impl {
    ImGuiContext* _ctx;

    Material _material{};

    Texture2d _fontTexture;
    vk::Sampler _fontSampler;

    uint32_t _frameIndex;
    uint32_t _frameCount;

    std::vector<Mesh> _frames;

    Impl() {
        IMGUI_CHECKVERSION();
        _ctx = ImGui::CreateContext();

        _ctx->IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        _ctx->IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        _ctx->IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

        _ctx->IO.BackendPlatformName = "imgui_impl_glfw";
        _ctx->IO.BackendRendererName = "imgui_impl_vulkan";

        // Keyboard mapping. ImGui will use those indices to peek into the _ctx->IO.KeysDown[] array.
    //    _ctx->IO.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    //    _ctx->IO.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    //    _ctx->IO.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    //    _ctx->IO.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    //    _ctx->IO.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    //    _ctx->IO.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    //    _ctx->IO.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    //    _ctx->IO.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    //    _ctx->IO.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    //    _ctx->IO.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    //    _ctx->IO.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    //    _ctx->IO.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    //    _ctx->IO.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    //    _ctx->IO.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    //    _ctx->IO.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    //    _ctx->IO.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    //    _ctx->IO.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    //    _ctx->IO.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    //    _ctx->IO.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    //    _ctx->IO.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    //    _ctx->IO.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    //    _ctx->IO.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

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
        _frameCount = Blaze::GetFrameCount();
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

    ~Impl() {
        ImGui::DestroyContext(_ctx);
        //    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
        //    {
        //        glfwDestroyCursor(_mouseCursors[cursor_n]);
        //        _mouseCursors[cursor_n] = NULL;
        //    }

        _frames.clear();
        Blaze::GetLogicalDevice().destroySampler(_fontSampler, nullptr);
    }

    void _createFontsTexture() {
        uint8_t *pixels;
        int width, height;
        _ctx->IO.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        _fontTexture = Texture2d(glm::u32(width), glm::u32(height)/*, vk::Format::eR8G8B8A8Unorm*/);
        _fontTexture.setPixels(std::span(reinterpret_cast<const glm::u8vec4*>(pixels), width * height));

        const auto samplerCreateInfo = vk::SamplerCreateInfo{
            .magFilter = vk::Filter::eLinear,
            .minFilter = vk::Filter::eLinear,
            .mipmapMode = vk::SamplerMipmapMode::eLinear,
            .addressModeU = vk::SamplerAddressMode::eRepeat,
            .addressModeV = vk::SamplerAddressMode::eRepeat,
            .addressModeW = vk::SamplerAddressMode::eRepeat,
            .maxAnisotropy = 1.0f,
            .minLod = -1000,
            .maxLod = 1000
        };

        _fontSampler = Blaze::GetLogicalDevice().createSampler(samplerCreateInfo, nullptr);

        _material.setTexture(0, _fontTexture, _fontSampler);
    }

    void setDisplaySize(std::pair<int, int> size) {
        _ctx->IO.DisplaySize = ImVec2(float(size.first), float(size.second));
    }

    void setDisplayScale(std::pair<float, float> scale) {
        _ctx->IO.DisplayFramebufferScale = ImVec2(scale.first, scale.second);
    }

    void setDeltaTime(const Timestamp& delta) {
        _ctx->IO.DeltaTime = static_cast<float>(delta.seconds());
    }

    void setMousePosition(std::pair<float, float> pos) {
        _ctx->IO.MousePos = ImVec2(pos.first, pos.second);
    }

    void _setupRenderState(ImDrawData* draw_data, vk::CommandBuffer cmd, Mesh* rb, int fb_width, int fb_height) {
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _material.getPipeline());
        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _material.getPipelineLayout(),
            0,
            _material.getDescriptorSets(),
            _material.getDynamicOffsets()
        );

        if (draw_data->TotalVtxCount > 0) {
            cmd.bindVertexBuffers(0, *rb->vertexBuffer, vk::DeviceSize{0});
            cmd.bindIndexBuffer(*rb->indexBuffer, 0, sizeof(ImDrawIdx) == 2 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);
        }

        cmd.setViewport(0, vk::Viewport{0, 0, float(fb_width), float(fb_height), 0, 1});

        const auto transform = std::array {
            2.0f / draw_data->DisplaySize.x,
            2.0f / draw_data->DisplaySize.y,
            -1.0f - draw_data->DisplayPos.x * (2.0f / draw_data->DisplaySize.x),
            -1.0f - draw_data->DisplayPos.y * (2.0f / draw_data->DisplaySize.y)
        };

        cmd.pushConstants(
            _material.getPipelineLayout(),
            vk::ShaderStageFlagBits::eVertex,
            0,
            std::span(transform).size_bytes(),
            transform.data()
        );
    }

    void draw(vk::CommandBuffer cmd) {
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

            auto vtx_dst = reinterpret_cast<ImDrawVert *>(frame.vertexBuffer.map());
            auto idx_dst = reinterpret_cast<ImDrawIdx *>(frame.indexBuffer.map());

            for (int n = 0; n < drawData->CmdListsCount; n++) {
                auto cmd_list = drawData->CmdLists[n];
                memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
                memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
                vtx_dst += cmd_list->VtxBuffer.Size;
                idx_dst += cmd_list->IdxBuffer.Size;
            }

            frame.vertexBuffer.unmap();
            frame.indexBuffer.unmap();
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

                    cmd.setScissor(0, 1, &scissor);
                    cmd.drawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
                }
            }
            global_idx_offset += cmd_list->IdxBuffer.Size;
            global_vtx_offset += cmd_list->VtxBuffer.Size;
        }

        _frameIndex = (_frameIndex + 1) % _frameCount;
    }

};

void UserInterface::init() {
    impl = blaze::make_internal<Impl>();
}

void UserInterface::setDisplaySize(std::pair<int, int> size) {
    impl->setDisplaySize(size);
}

void UserInterface::setDisplayScale(std::pair<float, float> scale) {
    impl->setDisplayScale(scale);
}

void UserInterface::setDeltaTime(const Timestamp& delta) {
    impl->setDeltaTime(delta);
}

void UserInterface::setMousePosition(std::pair<float, float> pos) {
    impl->setMousePosition(pos);
}

void UserInterface::begin() {
    ImGui::SetCurrentContext(impl->_ctx);
    ImGui::NewFrame();
}

void UserInterface::end() {
    ImGui::Render();
    ImGui::SetCurrentContext(nullptr);
}

void UserInterface::draw(vk::CommandBuffer cmd) {
    impl->draw(cmd);
}

void UserInterface::setMousePressed(int button, bool flag) {
    impl->_ctx->IO.MouseDown[button] = flag;
}