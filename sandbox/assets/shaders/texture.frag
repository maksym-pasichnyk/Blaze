#version 450 core

layout(location=0) out vec4 fColor;

layout(set=0, binding=0) uniform sampler2D sTexture;

layout(location=0) in struct {
    vec2 UV;
} In;

void main() {
    fColor = texture(sTexture, In.UV.st);
}