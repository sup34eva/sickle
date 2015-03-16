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
const vec3 ambientColor = vec3(0.1, 0.1, 0.1);
const vec3 lightColor = vec3(1, 1, 1);

layout(location = 0) out vec4 output;

const float PI = 3.14159265358979323846;

// Util functions
float sqr( float x ) {
    return x*x;
}

float Fresnel(float bias, float scale, float power, vec3 V, vec3 N) {
    return clamp(bias + scale * pow(1.0 + dot(V, N), power), 0.0, 1.0);
}

// Diffuse BRDF
vec3 Lambert( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    return vec3(1 / PI);
}

vec3 OrenNayar( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float R = 0.9;
    // calculate intermediary values
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);

    float angleVN = acos(NdotV);
    float angleLN = acos(NdotL);

    float alpha = max(angleVN, angleLN);
    float beta = min(angleVN, angleLN);
    float gamma = dot(V - N * dot(V, N), L - N * dot(L, N));

    float roughnessSquared = sqr(R);
    float roughnessSquared9 = (roughnessSquared / (roughnessSquared + 0.09));

    // calculate C1, C2 and C3
    float C1 = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
    float C2 = 0.45 * roughnessSquared9;

    if(gamma >= 0.0) {
        C2 *= sin(alpha);
    } else {
        C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
    }

    float powValue = (4.0 * alpha * beta) / (PI * PI);
    float C3  = 0.125 * roughnessSquared9 * powValue * powValue;

    // now calculate both main parts of the formula
    float A = gamma * C2 * tan(beta);
    float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);

    // put it all together
    float L1 = max(0.0, NdotL) * (C1 + A + B);

    // also calculate interreflection
    float twoBetaPi = 2.0 * beta / PI;
    //TODO: p is squared in this case... how to separate this?
    float L2 = 0.17 * max(0.0, NdotL) * (roughnessSquared / (roughnessSquared + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);

    // get the final color by summing both components
    return vec3(L1 + L2);
}

vec3 Schlick( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float R = 0.5;
    vec3 H = normalize(L+V);
    return vec3(Fresnel(R, 1.0, 5.0, H, L));
}

// Specular BRDF
vec3 BlinnPhong( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float n = 100;
    const bool divide_by_NdotL = true;

    vec3 H = normalize(L+V);
    float val = pow(max(0, dot(N, H)), n);
    if (divide_by_NdotL)
        val = val / dot(N, L);
    return vec3(val);
}

vec3 CookTorrance( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const bool include_F = true;
    const bool include_G = true;
    const float m = .1;
    const float f0 = .1;

    vec3 H = normalize( L + V );

    float NdotH = dot(N, H);
    float VdotH = dot(V, H);
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);
    float oneOverNdotV = 1.0 / NdotV;

    float M = sqr(m);
    float T = sqr(NdotH);
    float D = exp((T - 1) / (M * T)) / (PI * M * T * T);
    float F = Fresnel(f0, 1.0, 5.0, V, H);

    NdotH = NdotH + NdotH;
    float G = (NdotV < NdotL) ?
        ((NdotV*NdotH < VdotH) ?
            NdotH / VdotH :
            oneOverNdotV)
    :
        ((NdotL*NdotH < VdotH) ?
            NdotH*NdotL / (VdotH*NdotV) :
            oneOverNdotV);

    if (include_G) G = oneOverNdotV;
    float val = NdotH < 0 ? 0.0 : D * G ;

    if (include_F) val *= F;

    val = val / NdotL;
    return vec3(val);
}

vec3 Gaussian( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float S = 0.5;
    vec3 H = normalize( L + V );
    float NdotH = dot(N, H);
    float thetaH = acos(NdotH);
    float D = exp(-thetaH * thetaH / sqr(S));
    return vec3(D);
}

vec3 GGX( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float S = 0.5;
    vec3 H = normalize( L + V );
    float CosSquared = sqr(dot(N,H));
    float TanSquared = (1-CosSquared)/CosSquared;
    float D = (1.0 / PI) * sqr(S / (CosSquared * (sqr(S) + TanSquared)));
    return vec3(D);
}

vec3 TrowbridgeReitz( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const bool normalized = true;
    const float S = 0.5;

    vec3 H = normalize( L + V );
    float cSquared = sqr(S);
    float D = sqr(cSquared / (sqr(dot(N,H))*(cSquared-1)+1));
    if (normalized) {
        D *= 1/(cSquared*PI);
    }
    return vec3(D);
}

void main() {
    vec3 c = texture(color, UV).xyz;
    vec3 n = texture(normal, UV).xyz;
    vec3 x = texture(tangent, UV).xyz;
    vec3 y = texture(bitangent, UV).xyz;
    vec3 s = texture(shadow, UV).xyz;

    float NoL = clamp(dot(n, lightD), 0, 1);

    float visibility = texture(shadowMap, s);

    /* Couples Diffuse / Specular sympas:
      - Oren-Nayar / Blinn-Phong (Default)
      - Lambert / Cook-Torrance (Far Cry 3)
     */

    vec3 diffuse = OrenNayar(lightD, eyeD, n, x, y);
    vec3 specular = BlinnPhong(lightD, eyeD, n, x, y);

    vec3 radiance = (lightPower * (NoL * visibility) * ((diffuse * c) + specular)) + (ambientColor * c);
    output = vec4(radiance, 1);
}
