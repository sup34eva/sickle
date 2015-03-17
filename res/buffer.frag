#version 330 core

in vec3 fragColor;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;
in vec4 vertPos;

struct Material {
    float metallic;
    float subsurface;
    float specular;
    float roughness;

    float specularTint;
    float anisotropic;
    float sheen;
    float sheenTint;

    float clearcoat;
    float clearcoatGloss;
};

uniform Material material;

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 n;
layout(location = 2) out vec3 x;
layout(location = 3) out vec3 y;
layout(location = 4) out vec4 pos;
layout(location = 5) out vec4 matP1;
layout(location = 6) out vec4 matP2;
layout(location = 7) out vec4 matP3;

void main() {
    color = vec4(fragColor, 1);
    n = normalize(normal);
    x = normalize(tangent);
    y = normalize(bitangent);
    pos = vertPos;
    matP1 = vec4(material.metallic, material.subsurface, material.specular, material.roughness);
    matP2 = vec4(material.specularTint, material.anisotropic, material.sheen, material.sheenTint);
    matP3 = vec4(material.clearcoat, material.clearcoatGloss, 0, 0);
}
