#include "Mesh.hpp"
#include "Blaze.hpp"
#include "Display.hpp"
#include "Material.hpp"
#include "Timestamp.hpp"
#include "ThreadPool.hpp"
#include "TextureData.hpp"
#include "RenderPipeline.hpp"

#include <imgui.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

using Clock = std::chrono::high_resolution_clock;

float Time;
glm::vec2 Resolution;
glm::vec3 LightPosition;
glm::vec3 CameraPosition;
glm::mat3 CameraRotation;

static auto sdSmoothUnion(glm::f32 d1, glm::f32 d2, glm::f32 k) -> glm::f32 {
    const auto h = glm::clamp(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
    return glm::mix(d2, d1, h) - k * h * (1.0f - h);
}

static auto sdSphere(const glm::vec3& c, const glm::vec3& p, glm::f32 r) -> glm::f32 {
    return glm::length(c - p) - r;
}

static auto sdTorus(const glm::vec3& p, const glm::vec2& t) -> glm::f32 {
    const auto q = glm::vec2(glm::length(p.xz()) - t.x, p.y);
    return glm::length(q) - t.y;
}

static auto scene(const glm::vec3& p) -> glm::f32 {
    auto d = sdTorus(p, glm::vec2(1.0f, 0.2f));
    d = sdSmoothUnion(d, sdSphere(p, glm::vec3(0.0f, glm::sin(Time), 0.0f), 0.3f), 1.0f);
    return d;
}

static auto camera(const glm::vec3& cameraPos, const glm::vec3& lookAtPoint) -> glm::mat3 {
    const auto cd = glm::normalize(lookAtPoint - cameraPos);
    const auto cr = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cd));
    const auto cu = glm::normalize(glm::cross(cd, cr));
    return glm::mat3(-cr, cu, -cd);
}

static auto calcNormal(const glm::vec3& p) -> glm::vec3 {
    const auto e = glm::vec2(1.0f, -1.0f) * 0.0005f;
    return glm::normalize(
        e.xyy() * scene(p + e.xyy()) +
        e.yyx() * scene(p + e.yyx()) +
        e.yxy() * scene(p + e.yxy()) +
        e.xxx() * scene(p + e.xxx())
    );
}

static auto raymarch(const glm::vec3& ro, const glm::vec3& rd) -> glm::f32 {
    glm::f32 t = 0;
    for (int i = 0; i < 64; ++i) {
        const auto p = ro + rd * t;
        const auto d = scene(p);
        t += d;
        if (d < 0.001 || t > 10000.0f) {
            break;
        }
    }
    return t;
}

static auto mainImage(const glm::vec2& fragCoord) -> glm::vec4 {
    const auto uv = (fragCoord - 0.5f * Resolution) / Resolution.y;

    const auto ro = CameraPosition;
    const auto rd = CameraRotation * glm::normalize(glm::vec3(uv, -1.0f));

    const auto sd = raymarch(ro, rd);
    if (sd > 1000.0f) {
        return glm::vec4(0);
    }
    const auto p = ro + rd * sd;
    const auto n = calcNormal(p);
    const auto l = glm::normalize(LightPosition - p);
    const auto i = glm::clamp(glm::dot(n, l), 0.3f, 1.0f);
    const auto c = glm::vec3(1.0f, 0.0f, 0.0f) * i;
    return glm::vec4(c, 1.0f);
}

struct Game : Blaze::App {
    Mesh _mesh;
    Material _material;
    Texture2d _texture;
    vk::Sampler _sampler;
    TextureData _textureData{800, 600};

    float _updateTime = 0;
    bool _multithreading = true;

    void Init() override {
        _texture = Texture2d(_textureData.getExtent());
        _sampler = Blaze::GetLogicalDevice().createSampler(vk::SamplerCreateInfo{
            .magFilter = vk::Filter::eLinear,
            .minFilter = vk::Filter::eLinear,
            .mipmapMode = vk::SamplerMipmapMode::eLinear,
            .addressModeU = vk::SamplerAddressMode::eRepeat,
            .addressModeV = vk::SamplerAddressMode::eRepeat,
            .addressModeW = vk::SamplerAddressMode::eRepeat,
            .maxAnisotropy = 1.0f,
            .minLod = -1000,
            .maxLod = 1000
        }, nullptr);

        _material = Material::LoadFromResources("sandbox:materials/texture.material");
        _material.setTexture(0, _texture, _sampler);

        const auto vertices = std::array {
            glm::vec4{-1, -1, 0, 1},
            glm::vec4{-1, +1, 0, 0},
            glm::vec4{+1, +1, 1, 0},
            glm::vec4{+1, -1, 1, 1},
        };

        const auto indices = std::array{
            0, 1, 2,
            0, 2, 3
        };

        _mesh.setVertexBufferParams(vertices.size(), sizeof(glm::vec4));
        _mesh.setVertexBufferData(std::as_bytes(std::span(vertices)), 0);

        _mesh.setIndexBufferParams(indices.size(), sizeof(glm::u32));
        _mesh.setIndexBufferData(std::as_bytes(std::span(indices)), 0);
    }

    void Destroy() override {
        Blaze::GetLogicalDevice().destroySampler(_sampler, nullptr);
        _material = {};
        _texture = {};
        _mesh = {};
    }

    void Update() override {
        const auto time1 = Clock::now();
        const auto extent = _textureData.getExtent();

        Time += Blaze::GetDeltaTime();
        Resolution = glm::vec2(glm::uvec2(extent.width, extent.height));
        LightPosition = glm::vec3(
            glm::sin(glm::radians(180.0f) * Time) * 2.0f,
            0.5f,
            glm::cos(glm::radians(180.0f) * Time) * 2.0f
        );

        CameraPosition = glm::vec3(5, 5, 5);
        CameraRotation = camera(CameraPosition, glm::vec3(0));

        if (_multithreading) {
            auto pool = ThreadPool{};

            const auto xstep = extent.width;
            const auto ystep = 1;

            for (glm::u32 y = 0; y < extent.height; y += ystep) {
                for (glm::u32 x = 0; x < extent.width; x += xstep) {
                    pool.jobs.emplace([this, x0 = x, x1 = x + xstep, y0 = y, y1 = y + ystep] {
                        FillTexture(x0, x1, y0, y1);
                    });
                }
            }

            pool.start();
            pool.wait();
        } else {
            FillTexture(0, extent.width, 0, extent.height);
        }
        _texture.setPixels(_textureData.getPixels());
        const auto time2 = Clock::now();
        _updateTime = static_cast<float>(Timestamp{time2 - time1}.seconds());
    }

    void FillTexture(glm::u32 x0, glm::u32 x1, glm::u32 y0, glm::u32 y1) {
        for (glm::u32 y = y0; y < y1; ++y) {
            for (glm::u32 x = x0; x < x1; ++x) {
                _textureData.setPixel(x, y, mainImage(glm::vec2(x, y)));
            }
        }
    }

    void Render(CommandBuffer cmd) override {
        cmd.drawMesh(_mesh, _material);
    }

    void Overlay() override {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
        ImGui::Begin("Info");
        ImGui::TextUnformatted(fmt::format("DeltaTime: {:.3}s", Blaze::GetDeltaTime()).c_str());
        ImGui::TextUnformatted(fmt::format("UpdateTime: {:.3}s", _updateTime).c_str());
        ImGui::Checkbox("Multithreading", &_multithreading);
        ImGui::End();
    }
};

void Start(int argc, char** argv) {
    Blaze::Start("Sandbox", 800, 600, false, Game{});
}