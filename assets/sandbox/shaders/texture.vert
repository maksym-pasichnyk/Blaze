#version 450 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

layout(binding = 0) uniform UniformBufferObject {
    float uTime;
    vec3 uResolution;
    vec3 uLightPosition;
    vec3 uCameraPosition;
    mat4 uCameraRotation;
};

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out struct {
    vec2 UV;
} Out;

void main() {
    Out.UV = aUV;
    gl_Position = vec4(aPos, 0, 1);
}