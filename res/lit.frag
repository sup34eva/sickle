#version 330 core

in vec3 fragColor;
in vec3 normal;
in vec3 eyeDir;
in vec3 lightDir;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

uniform vec3 lightColor;
uniform float lightPower;
uniform vec3 ambientColor;

out vec4 color;

void main(){
    // Light
    vec3 n = normalize(normal);
    vec3 l = normalize(lightDir);
    float cosTheta = clamp(dot(n,l), 0, 1);
    vec3 direct = fragColor * lightColor * lightPower * cosTheta;

    // Ambient
    vec3 ambient = ambientColor * fragColor;

    // Specular
    vec3 specularColor = vec3(1, 1, 1);
    vec3 E = normalize(eyeDir);
    vec3 R = reflect(-l,n);
    float cosAlpha = clamp(dot(E, R), 0, 1);
    vec3 specular = specularColor * lightColor * lightPower * pow(cosAlpha, 5);

    // Final color
    color = vec4(ambient + direct + specular, 1);
}
