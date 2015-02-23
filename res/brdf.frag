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
    const float roughness = 0.5;

    // calculate intermediary values
    float NdotL = dot(N, L);
    float NdotV = dot(N, V);

    float angleVN = acos(NdotV);
    float angleLN = acos(NdotL);

    float alpha = max(angleVN, angleLN);
    float beta = min(angleVN, angleLN);
    float gamma = dot(V - N * dot(V, N), L - N * dot(L, N));

    float roughnessSquared = roughness * roughness;
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
    const float f0 = .5;

    vec3 H = normalize(L+V);
    return vec3(Fresnel(f0, 1.0, 5.0, H, L));
}

// Specular BRDF
vec3 BlinnPhong( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float n = 100;
    const bool divide_by_NdotL = true;

    vec3 H = normalize(L+V);
    float val = pow(max(0,dot(N,H)),n);
    if (divide_by_NdotL)
        val = val / dot(N,L);
    return vec3(val);
}

vec3 CookTorrance( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float m = .1;
    const float f0 = .1;
    const bool include_F = true;
    const bool include_G = true;

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

vec3 Gaussian( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y )
{
    const float c = .2;
    vec3 H = normalize( L + V );
    float NdotH = dot(N, H);
    float thetaH = acos(NdotH);
    float D = exp(-thetaH*thetaH/sqr(c));
    return vec3(D);
}

vec3 GGX( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y )
{
    const float alpha = .1;
    vec3 H = normalize( L + V );
    float CosSquared = sqr(dot(N,H));
    float TanSquared = (1-CosSquared)/CosSquared;
    float D = (1.0/PI) * sqr(alpha/(CosSquared * (alpha*alpha + TanSquared)));
    return vec3(D);
}

vec3 TrowbridgeReitz( vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y ) {
    const float c = .1;
    const bool normalized = true;

    vec3 H = normalize( L + V );
    float cSquared = sqr(c);
    float D = sqr(cSquared / (sqr(dot(N,H))*(cSquared-1)+1));
    if (normalized) {
        D *= 1/(c*c*PI);
    }
    return vec3(D);
}

void main() {
    vec3 l = normalize(lightDir);
    vec3 v = normalize(eyeDir);
    vec3 n = normalize(normal);
    vec3 x = normalize(tangent);
    vec3 y = normalize(bitangent);
    float NoL = clamp(dot(n, l), 0, 1);

    /* Couples Diffuse / Specular sympas:
      - Oren-Nayar / Blinn-Phong (Default)
      - Lambert / Cook-Torrance (Far Cry 3)
     */

    vec3 diffuse = OrenNayar(l, v, n, x, y);
    vec3 specular = BlinnPhong(l, v, n, x, y);

    vec3 radiance = (lightPower * NoL * ((diffuse * fragColor) + specular)) + (ambientColor * fragColor);
    color = vec4(radiance, 1);
}
