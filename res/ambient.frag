#version 330 core

in vec2 UV;

uniform sampler2D color;
uniform vec3 ambientColor;

layout(location = 0) out vec4 outColor;

void main(){
    outColor = texture(color, UV) * vec4(ambientColor, 1);
}
