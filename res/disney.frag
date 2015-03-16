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

struct Material {
    float metallic;
    float subsurface;
    float specular;
    float roughness;

    float specularTint;
    float anisotropic;
    float sheen;
    float sheenTint;

    float clearcoat;
    float clearcoatGloss;
};

const float lightPower = 1.0;
const vec3 ambientColor = vec3(0.1, 0.1, 0.1);
uniform Material material;
const vec3 lightColor = vec3(1, 1, 1);

layout(location = 0) out vec4 output;

const float PI = 3.14159265358979323846;

float sqr(float x) { return x * x; }

float SchlickFresnel(float u) {
    float m = clamp(1 - u, 0, 1);
    float m2 = m * m;
    return m2 * m2 * m; // pow(m,5)
}

float GTR1(float NdotH, float a) {
    if (a >= 1) return 1 / PI;
    float a2 = a * a;
    float t = 1 + (a2 - 1) * NdotH * NdotH;
    return (a2 - 1) / (PI * log(a2) * t);
}

float GTR2(float NdotH, float a) {
    float a2 = a * a;
    float t = 1 + (a2 - 1) * NdotH * NdotH;
    return a2 / (PI * t * t);
}

float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay) {
    return 1 / ( PI * ax * ay * sqr(sqr(HdotX / ax) + sqr(HdotY / ay) + NdotH * NdotH));
}

float smithG_GGX(float Ndotv, float alphaG) {
    float a = alphaG * alphaG;
    float b = Ndotv * Ndotv;
    return 1 / (Ndotv + sqrt(a + b - a * b));
}

vec3 mon2lin(vec3 x) {
    return vec3(pow(x[0], 2.2), pow(x[1], 2.2), pow(x[2], 2.2));
}

vec3 BRDF( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y, vec3 C) {
    float NdotL = max(dot(N, L), 0);
    float NdotV = max(dot(N, V), 0);
    vec3 H = normalize(L + V);
    float NdotH = dot(N, H);
    float LdotH = dot(L, H);
    vec3 Cdlin = mon2lin(C);
    float Cdlum = .3 * Cdlin[0] + .6 * Cdlin[1] + .1 * Cdlin[2]; // luminance approx.
    vec3 Ctint = Cdlum > 0 ? Cdlin/Cdlum : vec3(1); // normalize lum. to isolate hue+sat
    vec3 Cspec0 = mix(material.specular * .08 * mix(vec3(1), Ctint, material.specularTint), Cdlin, material.metallic);
    vec3 Csheen = mix(vec3(1), Ctint, material.sheenTint);
    // Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
    // and mix in diffuse retro-reflection based on roughness
    float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
    float Fd90 = 0.5 + 2 * LdotH * LdotH * material.roughness;
    float Fd = mix(1, Fd90, FL) * mix(1, Fd90, FV);
    // Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
    // 1.25 scale is used to (roughly) preserve albedo
    // Fss90 used to "flatten" retroreflection based on roughness
    float Fss90 = LdotH * LdotH * material.roughness;
    float Fss = mix(1, Fss90, FL) * mix(1, Fss90, FV);
    float ss = 1.25 * (Fss * (1 / (NdotL + NdotV) - .5) + .5);
    // specular
    float aspect = sqrt(1 - material.anisotropic * .9);
    float ax = max(.001, sqr(material.roughness) / aspect);
    float ay = max(.001, sqr(material.roughness) * aspect);
    float Ds = GTR2_aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
    float FH = SchlickFresnel(LdotH);
    vec3 Fs = mix(Cspec0, vec3(1), FH);
    float roughg = sqr(material.roughness * .5 + .5);
    float Gs = smithG_GGX(NdotL, roughg) * smithG_GGX(NdotV, roughg);
    // sheen
    vec3 Fsheen = FH * material.sheen * Csheen;
    // clearcoat (ior = 1.5 -> F0 = 0.04)
    float Dr = GTR1(NdotH, mix(.1, .001, material.clearcoatGloss));
    float Fr = mix(.04, 1, FH);
    float Gr = smithG_GGX(NdotL, .25) * smithG_GGX(NdotV, .25);
    return ((1/PI) * mix(Fd, ss, material.subsurface) * Cdlin + Fsheen)
            * (1 - material.metallic)
            + Gs * Fs * Ds + .25 * material.clearcoat * Gr * Fr * Dr;
}

void main() {
    vec3 c = texture(color, UV).xyz;
    vec3 n = texture(normal, UV).xyz;
    vec3 x = texture(tangent, UV).xyz;
    vec3 y = texture(bitangent, UV).xyz;
    vec3 s = texture(shadow, UV).xyz;

    float NoL = clamp(dot(n, lightD), 0, 1);

    float visibility = texture(shadowMap, s);

    vec3 light = (lightPower * NoL * BRDF(lightD, eyeD, n, x, y, c)) + (ambientColor * c);
    output = vec4(light, 1);
}
