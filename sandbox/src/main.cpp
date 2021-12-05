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

static auto sphere(const glm::vec3& ro, const glm::vec3& rd, const glm::vec3& c, float r) -> float {
    const auto L = c - ro;
    const auto tca = glm::dot(L, rd);
    const auto d2 = glm::dot(L, L) - tca * tca;
    if (d2 > r * r) {
        return -1.0f;
    }
    const auto thc = glm::sqrt(r * r - d2);
    const auto t0 = tca - thc;
    const auto t1 = tca + thc;
    return t0 >= 0 ? t0 : t1 >= 0 ? t1 : -1.0f;
}

using Clock = std::chrono::high_resolution_clock;

struct Game : Blaze::App {
    Material _material;

    Texture2d _texture;
    vk::Sampler _sampler;
    TextureData _textureData{800 * 4, 600 * 4};

    Mesh _mesh;
    float _time = 0;
    float _angle = 0;
    bool _multithreading = true;

    void Init() override {
        _material = Material::LoadFromResources("sandbox:materials/texture.material");
        _texture = Texture2d(_textureData.getExtent());

        const auto samplerCreateInfo = vk::SamplerCreateInfo{
//            .magFilter = vk::Filter::eLinear,
//            .minFilter = vk::Filter::eLinear,
//            .mipmapMode = vk::SamplerMipmapMode::eLinear,
            .addressModeU = vk::SamplerAddressMode::eRepeat,
            .addressModeV = vk::SamplerAddressMode::eRepeat,
            .addressModeW = vk::SamplerAddressMode::eRepeat,
            .maxAnisotropy = 1.0f,
            .minLod = -1000,
            .maxLod = 1000
        };

        _sampler = Blaze::GetLogicalDevice().createSampler(samplerCreateInfo, nullptr);
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
        const auto extent = _textureData.getExtent();
        const auto f_extent = glm::vec2(glm::uvec2(extent.width, extent.height));
        const auto aspect = glm::vec2(f_extent.x, f_extent.y) / glm::vec2(f_extent.y, f_extent.y);


        const auto ro = glm::vec3(0, 0, -3);
        const auto light = glm::normalize(glm::vec3(glm::sin(_angle), 0.5f, glm::cos(_angle)));

        const auto time1 = Clock::now();

        if (_multithreading) {
            ThreadPool pool{};

            const auto xstep = extent.width;
            const auto ystep = 1;

            for (glm::u32 y = 0; y < extent.height; y += ystep) {
                for (glm::u32 x = 0; x < extent.width; x += xstep) {
                    pool.jobs.emplace([&, x0 = x, x1 = x + xstep, y0 = y, y1 = y + ystep] {
                        for (glm::u32 y = y0; y < y1; ++y) {
                            for (glm::u32 x = x0; x < x1; ++x) {
                                _textureData.setPixel(x, y, glm::u8vec4(0));

                                const auto uv = (glm::vec2(glm::uvec2(x, y)) / f_extent * 2.0f - 1.0f) * aspect;
                                const auto rd = glm::normalize(glm::vec3(uv, 1));
                                const auto s = sphere(ro, rd, glm::vec3(0), 1.0f);

                                if (s >= 0.0f) {
                                    const auto p = glm::normalize(ro + rd * s);
                                    const auto i = glm::clamp(glm::dot(p, light), 0.0f, 1.0f) + 0.2F;
                                    const auto c = glm::vec3(1.0f, 0.0f, 0.0f) * i;
                                    _textureData.setPixel(x, y, glm::vec4(c, 1.0f));
                                }
                            }
                        }
                    });
                }
            }

            pool.start();
            pool.wait();
        } else {
            for (glm::u32 y = 0; y < extent.height; ++y) {
                for (glm::u32 x = 0; x < extent.width; ++x) {
                    _textureData.setPixel(x, y, glm::u8vec4(0));

                    const auto uv = (glm::vec2(glm::uvec2(x, y)) / f_extent * 2.0f - 1.0f) * aspect;
                    const auto rd = glm::normalize(glm::vec3(uv, 1));

                    const auto s = sphere(ro, rd, glm::vec3(0), 1.0f);

                    if (s >= 0.0f) {
                        const auto p = glm::normalize(ro + rd * s);
                        const auto i = glm::clamp(glm::dot(p, light), 0.0f, 1.0f) + 0.2F;
                        const auto c = glm::vec3(1.0f, 0.0f, 0.0f) * i;
                        _textureData.setPixel(x, y, glm::vec4(c,  1.0f));
                    }
                }
            }
        }
        _texture.setPixels(_textureData.getPixels());
        const auto time2 = Clock::now();

        _time = static_cast<float>(Timestamp{time2 - time1}.seconds());
        _angle += glm::radians(1.0f) * Blaze::GetDeltaTime() * 180.0f;
    }

    void Render(CommandBuffer cmd) override {
        cmd.drawMesh(_mesh, _material);
    }

    void Overlay() override {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
        ImGui::Begin("Info");
        ImGui::TextUnformatted(fmt::format("DeltaTime: {:.3}s", Blaze::GetDeltaTime()).c_str());
        ImGui::TextUnformatted(fmt::format("UpdateTexture: {:.3}s", _time).c_str());
        ImGui::Checkbox("Multithreading", &_multithreading);
        ImGui::End();
    }
};

void Start(int argc, char** argv) {
    Blaze::Start("Sandbox", 800, 600, false, Game{});
}