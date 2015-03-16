#version 330 core

in vec2 UV;

uniform sampler2D color;

out vec4 output;

void main(){
    output = texture(color, UV);
}
