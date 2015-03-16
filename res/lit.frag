#version 330 core

in vec2 UV;

uniform sampler2D color;
uniform sampler2D normal;
uniform sampler2D tangent;
uniform sampler2D bitangent;
uniform sampler2D shadow;
uniform sampler2DShadow shadowMap;
uniform vec3 lightD;
uniform vec3 eyeD;

const float lightPower = 1.0;
const vec3 lightColor = vec3(1, 1, 1);
const vec3 ambientColor = vec3(0.1, 0.1, 0.1);

layout(location = 0) out vec4 output;

void main(){
    vec3 fragColor = texture(color, UV).xyz;

    // Light
    vec3 n = texture(normal, UV).xyz;
    vec3 l = lightD;
    float cosTheta = clamp(dot(n, l), 0, 1);

    vec3 coord = texture(shadow, UV).xyz;
    float visibility = texture(shadowMap, coord);

    vec3 direct = fragColor * lightPower;

    // Ambient
    vec3 ambient = ambientColor * fragColor;

    // Specular
    vec3 R = reflect(-l, n);
    float cosAlpha = clamp(dot(eyeD, R), 0, 1);
    vec3 specular = lightColor * lightPower * pow(cosAlpha, 5);

    // Final color
    output = vec4(ambient + (visibility * cosTheta) * (direct + specular), 1);
}
