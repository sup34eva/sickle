#version 330 core

layout(location = 0) in vec3 pos;

out vec2 UV;

void main() {
    UV = (pos.xy * 0.5) + 0.5;
    gl_Position.xyz = pos;
}
