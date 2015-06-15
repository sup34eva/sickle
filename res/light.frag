#version 330 core

in vec2 UV;

uniform sampler2D color;
uniform sampler2D normal;
uniform sampler2D tangent;
uniform sampler2D bitangent;
uniform sampler2D vertPos;
uniform sampler2D matProp1;
uniform sampler2D matProp2;
uniform sampler2D matProp3;
uniform sampler2DShadow shadowMap;

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

struct Light {
    int type;
    vec3 location;
    vec3 orientation;
    vec3 color;
    float power;
    float falloff;
    float inner;
    float outer;
};

uniform Light light;
uniform vec3 eyeD;
uniform mat4 vDepth;
uniform mat4 pDepth;

layout(location = 0) out vec4 outColor;

const vec2 poissonDisk[4] = vec2[](
    vec2(-0.94201624,  -0.39906216),
    vec2( 0.94558609,  -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2( 0.34495938,   0.29387760)
);

const float PI = 3.14159265358979323846;

float sqr(float x) { return x * x; }

vec3 ConstantBiasScale(vec3 vec, float bias, float scale) {
    return (vec + bias) * scale;
}

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

vec3 BRDF( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y, vec3 C, Material material) {
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

Material getMat() {
    Material mat;
    vec4 prop1 = texture(matProp1, UV);
    vec4 prop2 = texture(matProp2, UV);
    vec4 prop3 = texture(matProp3, UV);

    mat.metallic = prop1.x;
    mat.subsurface = prop1.y;
    mat.specular = prop1.z;
    mat.roughness = prop1.w;

    mat.specularTint = prop2.x;
    mat.anisotropic = prop2.y;
    mat.sheen = prop2.z;
    mat.sheenTint = prop2.w;

    mat.clearcoat = prop3.x;
    mat.clearcoatGloss = prop3.y;

    return mat;
}

void main() {
    vec3 c = texture(color, UV).xyz;
    vec3 n = texture(normal, UV).xyz;
    vec3 x = texture(tangent, UV).xyz;
    vec3 y = texture(bitangent, UV).xyz;
    vec3 pos = texture(vertPos, UV).xyz;
    vec4 s = (pDepth * vDepth) * vec4(pos, 1);

    vec3 lightD;
    float falloff = 1.0;
    if(light.type == 0) {
        lightD = light.orientation;
    } else if(light.type == 1) {
        lightD = normalize(light.location - pos);
        float angle = dot(lightD, light.orientation);
        float dist = distance(pos, light.location);
        falloff = sqr(clamp(1.0 - (dist / light.falloff), 0.0, 1.0));
        falloff *= clamp((angle - light.outer) / (light.inner - light.outer), 0.0, 1.0);
    }

    float NoL = clamp(dot(n, lightD), 0, 1);

    float bias = clamp(0.005 * tan(acos(NoL)), 0, 0.01);
    vec3 shadowCoord = ConstantBiasScale(s.xyz / s.w, 1.0, 0.5);
    float visibility = 0.0;
    for (int i=0; i < 4; i++){
        vec3 tempS = vec3(shadowCoord.xy + poissonDisk[i] / 700.0, shadowCoord.z);
        if(tempS.x >= 0 && tempS.x <= 1 && tempS.y >= 0 && tempS.y <= 1)
            visibility += texture(shadowMap, tempS, bias) * 0.2;
        else
            visibility += 0.2;
    }

    Material mat = getMat();

    vec3 radiance = light.power * light.color * (NoL * visibility * falloff) * BRDF(lightD, eyeD, n, x, y, c, mat);
    outColor = vec4(radiance, 1);
    //output = vec4(light.power * vec3(falloff, visibility, NoL), 1);
}
