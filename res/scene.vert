#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec3 vertexTangent;
in vec3 vertexBitangent;

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
