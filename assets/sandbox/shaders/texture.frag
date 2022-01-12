#version 450 core

layout(location = 0) out vec4 fColor;
layout(location = 0) in struct {
    vec2 UV;
} In;

layout(binding = 1) uniform sampler2D sTexture;

layout(push_constant) uniform MaterialPropertyBlock {
    float Time;
    vec3 Resolution;
    vec3 LightPosition;
    vec3 CameraPosition;
    mat4 CameraRotation;
};

float sdSmoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5f + 0.5f * (d2 - d1) / k, 0.0f, 1.0f);
    return mix(d2, d1, h) - k * h * (1.0f - h);
}

float sdSphere(in vec3 c, in vec3 p, float r) {
    return length(c - p) - r;
}

float sdTorus(in vec3 p, in vec2 t) {
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float scene(in vec3 p)  {
    float d = sdTorus(p, vec2(1.0f, 0.2f));
    d = sdSmoothUnion(d, sdSphere(p, vec3(0.0f, sin(Time), 0.0f), 0.3f), 1.0f);
    return d;
}

mat3 camera(in vec3 cameraPos, in vec3 lookAtPoint) {
    vec3 cd = normalize(lookAtPoint - cameraPos);
    vec3 cr = normalize(cross(vec3(0.0f, 1.0f, 0.0f), cd));
    vec3 cu = normalize(cross(cd, cr));
    return mat3(-cr, cu, -cd);
}

vec3 calcNormal(in vec3 p) {
    vec2 e = vec2(1.0f, -1.0f) * 0.0005f;
    return normalize(
        e.xyy * scene(p + e.xyy) +
        e.yyx * scene(p + e.yyx) +
        e.yxy * scene(p + e.yxy) +
        e.xxx * scene(p + e.xxx)
    );
}

float raymarch(in vec3 ro, in vec3 rd) {
    float t = 0;
    for (int i = 0; i < 64; ++i) {
        vec3 p = ro + rd * t;
        float d = scene(p);
        t += d;
        if (d < 0.001 || t > 10000.0f) {
            break;
        }
    }
    return t;
}

vec4 mainImage(vec2 fragCoord) {
    vec2 uv = (fragCoord * 2.0f - 1.0f) * Resolution.xy / Resolution.yy;//(fragCoord - 0.5f * Resolution) / Resolution.y;
    vec3 ro = CameraPosition.xyz;
    vec3 rd = mat3(CameraRotation) * normalize(vec3(uv, -1.0f));

    float sd = raymarch(ro, rd);
    if (sd > 1000.0f) {
        return vec4(0);
    }
    vec3 p = ro + rd * sd;
    vec3 n = calcNormal(p);
    vec3 l = normalize(LightPosition.xyz - p);
    float i = clamp(dot(n, l), 0.3f, 1.0f);
    vec3 c = vec3(1.0f, 0.0f, 0.0f) * i;
    return vec4(c, 1.0f);
}

void main() {
    fColor = mainImage(In.UV.st);
//    texture(sTexture, In.UV.st);
}