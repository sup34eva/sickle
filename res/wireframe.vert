#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragColor;
out vec3 fragPos;

void main(){
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = projection * view * model * v;
    fragColor = vertexColor;
}
