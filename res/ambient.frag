#version 330 core

in vec2 UV;

uniform sampler2D color;
uniform vec3 ambientColor;

out vec4 output;

void main(){
    output = texture(color, UV) * vec4(ambientColor, 1);
}
