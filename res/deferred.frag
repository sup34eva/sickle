#version 330 core

in vec2 UV;

uniform sampler2D sceneBuffer;

out vec4 color;

void main() {
    color = texture2D(sceneBuffer, UV);
}
