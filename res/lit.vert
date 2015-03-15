#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec3 vertexTangent;
in vec3 vertexBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 MVP;
uniform mat4 dMVP;
uniform vec3 lightD;

out vec3 fragColor;
out vec3 normal;
out vec3 eyeDir;
out vec3 lightDir;
out vec2 texCoord;
out vec3 tangent;
out vec3 bitangent;
out vec4 shadow;

void main() {
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = MVP * v;
    shadow = dMVP * v;
    fragColor = vertexColor;
    eyeDir = vec3(0,0,0) - vec3(view * model * v);
    lightDir = vec3(view * vec4(lightD, 0));
    normal = vec3(view * model * vec4(vertexNormal, 0));
    texCoord = vertexUV;
    tangent = vertexTangent;
    bitangent = vertexBitangent;
}
