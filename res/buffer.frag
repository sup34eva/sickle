#version 330 core

in vec3 fragColor;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;
in vec4 shadow;

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 n;
layout(location = 2) out vec3 x;
layout(location = 3) out vec3 y;
layout(location = 4) out vec3 uv;

vec3 ConstantBiasScale(vec3 vec, float bias, float scale) {
    return (vec + bias) * scale;
}

void main() {
    color = vec4(fragColor, 1);
    n = normalize(normal);
    x = normalize(tangent);
    y = normalize(bitangent);
    uv = ConstantBiasScale(shadow.xyz / shadow.w, 1.0, 0.5);
}
