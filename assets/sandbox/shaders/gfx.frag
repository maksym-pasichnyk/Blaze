#version 450 core

layout(location = 0) out vec4 out_color;

//layout(binding = 0) uniform sampler2D sTexture;

layout(location=0) in struct {
    vec4 color;
//    vec2 UV;
} v2f;

void main() {
    out_color = v2f.color;// * texture(sTexture, In.UV.st);
}