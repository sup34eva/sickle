#version 430 core

in vec3 vertexPosition;
in float vertexColor;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec3 vertexTangent;
in vec3 vertexBitangent;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 MVP;
layout(location = 2) uniform vec4 colors[6];

out vec3 fragColor;
out vec3 normal;
out vec2 texCoord;
out vec3 tangent;
out vec3 bitangent;
out vec4 vertPos;

void main() {
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = MVP * v;
    vertPos = model * v;
    fragColor = colors[int(vertexColor)].rgb;
    normal = vec3(model * vec4(vertexNormal, 0));
    texCoord = vertexUV;
    tangent = vec3(model * vec4(vertexTangent, 0));
    bitangent = vec3(model * vec4(vertexBitangent, 0));
}
