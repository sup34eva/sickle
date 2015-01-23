#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;

uniform mat4 MVP;

out vec3 fragColor;

void main(){
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = MVP * v;
    fragColor = vertexColor;
}
