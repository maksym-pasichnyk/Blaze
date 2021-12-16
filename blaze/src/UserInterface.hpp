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
    explicit UserInterface(glm::u32 frameCount);
    ~UserInterface();

    void setDeltaTime(float delta);
    void setDisplaySize(const glm::ivec2& size);
    void setDisplayScale(const glm::vec2& scale);
    void setMousePosition(const glm::vec2& pos);
    void setMousePressed(int button, bool flag);

    void begin();
    void end();
    void draw(CommandBuffer cmd);

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
