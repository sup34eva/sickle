#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexUV;
layout(location = 4) in vec3 vertexTangent;
layout(location = 5) in vec3 vertexBitangent;

uniform mat4 model;
uniform mat4 MVP;

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
    fragColor = vertexColor;
    normal = vec3(model * vec4(vertexNormal, 0));
    texCoord = vertexUV;
    tangent = vertexTangent;
    bitangent = vertexBitangent;
}
