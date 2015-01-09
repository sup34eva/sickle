#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;

out vec3 fragColor;
out vec3 lightDir;
out vec3 normal;
out vec3 eyeDir;
out float lightDist;

void main(){
    vec4 v = vec4(vertexPosition, 1);
    mat4 MVP = projection * view * model;
    gl_Position = MVP * v;
    fragColor = vertexColor;

    // Light dist
    vec3 position_w = vec3(model * v);
    lightDist = distance(position_w, lightPosition);

    // Eye dir
    vec3 vertexPosition_c = vec3(view * model * v);
    eyeDir = vec3(0,0,0) - vertexPosition_c;

    // Light dir
    vec3 lightPosition_c = vec3(view * vec4(lightPosition, 1));
    lightDir = lightPosition_c + eyeDir;

    // Normal
    normal = vec3(view * model * vec4(vertexNormal, 0));
}
