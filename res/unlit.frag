#version 330 core

in vec2 UV;

uniform sampler2D color;

layout(location = 0) out vec4 outColor;

void main(){
    outColor = texture(color, UV);
}
