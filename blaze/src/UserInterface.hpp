#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "CommandBuffer.hpp"

struct ImDrawData;
struct ImGuiContext;
struct UserInterface {
    explicit UserInterface(size_t frameCount);
    ~UserInterface();

    void SetDeltaTime(float delta);
    void SetDisplaySize(const glm::ivec2& size);
    void SetDisplayScale(const glm::vec2& scale);
    void SetMousePosition(const glm::vec2& pos);
    void SetMousePressed(int button, bool flag);
    void SetKeyPressed(int button, bool flag);

    void Draw(CommandBuffer cmd);

    void SetCurrentContext();
    auto WantCaptureMouse() -> bool;
    void AddInputCharacter(char ch);
    void AddScrollMouse(float x, float y);

private:
    void _createFontsTexture();
    void _setupRenderState(ImDrawData* draw_data, CommandBuffer cmd, Mesh* rb, int fb_width, int fb_height);

    ImGuiContext* _ctx;
    Material _material{};
    Texture2D _fontTexture;
    uint32_t _frameIndex;
    uint32_t _frameCount;
    std::vector<Mesh> _frames;
};
