#include "Mesh.hpp"
#include "Blaze.hpp"
#include "Material.hpp"
#include "TextureData.hpp"
#include "../../blaze/internal/VulkanGraphicsBuffer.hpp"
#include "../../blaze/internal/VulkanMaterial.hpp"
#include <Texture.hpp>
#include <Time.hpp>

#include <imgui.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

struct MaterialPropertyBlock {
    alignas(16) float Time;
    alignas(16) glm::vec2 Resolution;
    alignas(16) glm::vec3 LightPosition;
    alignas(16) glm::vec3 CameraPosition;
    alignas(16) glm::mat4 CameraRotation;
};

float iTime;
glm::vec2 iResolution;
glm::vec3 iLightPosition;
glm::vec3 iCameraPosition;
glm::mat3 iCameraRotation;

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
    d = sdSmoothUnion(d, sdSphere(p, glm::vec3(0.0f, glm::sin(iTime), 0.0f), 0.3f), 1.0f);
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
    const auto uv = (fragCoord - 0.5f * iResolution) / iResolution.y;

    const auto ro = iCameraPosition;
    const auto rd = iCameraRotation * glm::normalize(glm::vec3(uv, -1.0f));

    const auto sd = raymarch(ro, rd);
    if (sd > 1000.0f) {
        return glm::vec4(0);
    }
    const auto p = ro + rd * sd;
    const auto n = calcNormal(p);
    const auto l = glm::normalize(iLightPosition - p);
    const auto i = glm::clamp(glm::dot(n, l), 0.3f, 1.0f);
    const auto c = glm::vec3(1.0f, 0.0f, 0.0f) * i;
    return glm::vec4(c, 1.0f);
}

struct Game : Blaze::Application {
    Mesh _mesh;
    Material _material;
    Texture2D _texture;
    TextureData _textureData = TextureData::Create(800, 600);

    GraphicsBuffer _constantBuffer;

    bool _multithreading = true;

    void Init() override {
        _texture = Texture2D(800, 600, vk::Format::eR8G8B8A8Unorm);

        _constantBuffer = GraphicsBuffer(GraphicsBuffer::Target::Constant, sizeof(MaterialPropertyBlock));
        _material = Material::LoadFromResources("sandbox:materials/texture.material");
        _material.SetTexture(1, _texture);
        _material.SetConstantBuffer(0, _constantBuffer);

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
        _mesh = {};
        _texture = {};
        _material = {};
        _constantBuffer = {};
    }

    void Update() override {
        iTime += Time::getDeltaTime();
        iResolution = glm::vec2(_textureData.getDimension());
        iLightPosition = glm::vec3(
            glm::sin(glm::radians(180.0f) * iTime) * 2.0f,
            0.5f,
            glm::cos(glm::radians(180.0f) * iTime) * 2.0f
        );
        iCameraPosition = glm::vec3(5, 5, 5);
        iCameraRotation = camera(iCameraPosition, glm::vec3(0, 0, 0));

//        if (_multithreading) {
//            auto pool = ThreadPool{};
//
//            const auto xstep = extent.width;
//            const auto ystep = 1;
//
//            for (glm::u32 y = 0; y < extent.height; y += ystep) {
//                for (glm::u32 x = 0; x < extent.width; x += xstep) {
//                    pool.jobs.emplace([this, x0 = x, x1 = x + xstep, y0 = y, y1 = y + ystep] {
//                        FillTexture(x0, x1, y0, y1);
//                    });
//                }
//
//            }
//            pool.start();
//            pool.wait();
//        } else {
//            FillTexture(0, extent.width, 0, extent.height);
//        }
//        _texture.setPixels(_textureData.getPixels());
    }

    void FillTexture(glm::u32 x0, glm::u32 x1, glm::u32 y0, glm::u32 y1) {
        for (glm::u32 y = y0; y < y1; ++y) {
            for (glm::u32 x = x0; x < x1; ++x) {
                _textureData.setPixel(x, y, mainImage(glm::vec2(x, y)));
            }
        }
    }

    void Draw(CommandBuffer cmd) override {
        auto block = MaterialPropertyBlock {
            .Time = iTime,
            .Resolution = iResolution,
            .LightPosition = iLightPosition,
            .CameraPosition = iCameraPosition,
            .CameraRotation = iCameraRotation
        };

        auto vk_material = static_cast<VulkanMaterial*>(_material.GetNativeHandlePtr());
        _constantBuffer.setData(&block, sizeof(MaterialPropertyBlock), 0);

        auto _cmd = *cmd;
        _cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, vk_material->pipeline);
        _cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            vk_material->pipelineLayout,
            0,
            vk_material->descriptorSets,
            vk_material->dynamicOffsets
        );
        _cmd.pushConstants(
            vk_material->pipelineLayout,
            vk::ShaderStageFlagBits::eFragment,
            0,
            sizeof(MaterialPropertyBlock),
            &block
        );

        auto vtx_ptr = static_cast<VulkanGraphicsBuffer*>(_mesh.getNativeVertexBufferPtr());
        auto idx_ptr = static_cast<VulkanGraphicsBuffer*>(_mesh.getNativeIndexBufferPtr());

        _cmd.bindVertexBuffers(0, vtx_ptr->buffer, vk::DeviceSize{0});
        _cmd.bindIndexBuffer(idx_ptr->buffer, 0, vk::IndexType::eUint32);
        _cmd.drawIndexed(_mesh.getIndexCount(), 1, 0, 0, 0);
//        cmd.drawMesh(_mesh, _material);
    }

    void DrawUI() override {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
        ImGui::Begin("Info");
        ImGui::TextUnformatted(fmt::format("DeltaTime: {:.3}s", Time::getDeltaTime()).c_str());
        ImGui::Checkbox("Multithreading", &_multithreading);
        ImGui::End();
    }
};

auto Blaze::CreateApplication() -> std::unique_ptr<Application> {
    return std::make_unique<Game>();
}