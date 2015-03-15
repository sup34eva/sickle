#version 330 core

in vec3 fragColor;
in vec3 normal;
in vec3 eyeDir;
in vec3 lightDir;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;
in vec4 shadow;

uniform mat4 view;
uniform mat4 model;
uniform mat4 MVP;
uniform mat4 dMVP;
uniform vec3 lightColor;
uniform float lightPower;
uniform vec3 ambientColor;
uniform sampler2DShadow shadowMap;

layout(location = 0) out vec4 color;

void main(){
    // Light
    vec3 n = normalize(normal);
    vec3 l = normalize(lightDir);
    float cosTheta = clamp(dot(n,l), 0, 1);
    vec3 uv = (((shadow.xyz/ shadow.w) * 0.5) + 0.5);
    float visibility = texture(shadowMap, uv);
    vec3 direct = fragColor * lightColor * lightPower * (cosTheta * visibility);

    // Ambient
    vec3 ambient = ambientColor * fragColor;

    // Specular
    vec3 specularColor = vec3(1, 1, 1);
    vec3 E = normalize(eyeDir);
    vec3 R = reflect(-l,n);
    float cosAlpha = clamp(dot(E, R), 0, 1);
    vec3 specular = specularColor * lightColor * lightPower * pow(cosAlpha, 5);

    // Final color
    color = vec4(ambient + (visibility * direct) + (visibility * specular), 1);
    //color = vec4(uv.xy, visibility, 1);
}
