#version 330 core

in vec3 fragColor;
in vec3 normal;
in vec3 eyeDir;
in vec3 lightDir;

uniform vec3 lightColor;
uniform float lightPower;
uniform vec3 ambientColor;

out vec4 color;

void main(){
    // Light
    vec3 n = normalize(normal);
    vec3 l = normalize(lightDir);
    float cosTheta = clamp(dot(n,l), 0, 1);

    // Ambient
    vec3 ambientColor = ambientColor * fragColor;

    // Specular
    vec3 specularColor = vec3(1, 1, 1);
    vec3 E = normalize(eyeDir);
    vec3 R = reflect(-l,n);
    float cosAlpha = clamp(dot(E, R), 0, 1);

    // Final color
    color = vec4(ambientColor +
    fragColor * lightColor * lightPower * cosTheta +
    specularColor * lightColor * lightPower * pow(cosAlpha, 5), 1);
}
