#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 MVP;
uniform vec3 lightD;

out vec3 fragColor;
out vec3 normal;
out vec3 eyeDir;
out vec3 lightDir;

void main(){
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = MVP * v;
    fragColor = vertexColor;
    eyeDir = vec3(0,0,0) - vec3(view * model * v);
    lightDir = vec3(view * vec4(lightD, 0));
    normal = vec3(view * model * vec4(vertexNormal, 0));
}
