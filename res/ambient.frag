#version 420 core

in vec2 UV;

uniform sampler2D color;
uniform sampler2D normal;
uniform sampler2D vertPos;
uniform sampler2D depth;

struct AmbientOcclusion {
    vec2 kernelSize;
    float threshold;
    float maxDist;
};

uniform vec3 ambientColor;
uniform AmbientOcclusion AO;

layout(location = 0) out vec4 outColor;

const int length = 16;
const vec2 poissonDisk[length] = {
    { -0.94201624,  -0.39906216 },
    {  0.94558609,  -0.76890725 },
    { -0.094184101, -0.92938870 },
    {  0.34495938,   0.29387760 },
    { -0.91588581,   0.45771432 },
    { -0.81544232,  -0.87912464 },
    { -0.38277543,   0.27676845 },
    {  0.97484398,   0.75648379 },
    {  0.44323325,  -0.97511554 },
    {  0.53742981,  -0.47373420 },
    { -0.26496911,  -0.41893023 },
    {  0.79197514,   0.19090188 },
    { -0.24188840,   0.99706507 },
    { -0.81409955,   0.91437590 },
    {  0.19984126,   0.78641367 },
    {  0.14383161,  -0.14100790 }
};

float OneMinus(float x) { return 1.0 - x; }

void main() {
    float stencil = texture(depth, UV).b;
    outColor = vec4(stencil, 0, 0, 1);
    if(stencil < 1) {
        vec3 c = texture(color, UV).rgb;
        vec3 n = texture(normal, UV).rgb;
        vec3 pos = texture(vertPos, UV).xyz;

        float occlusion = 0.0;
        for(int i = 0; i < length; i++) {
            vec2 offset = UV + (poissonDisk[i] * AO.kernelSize);
            vec3 samplePos = texture(vertPos, offset).xyz;
            vec3 sampleDir = normalize(samplePos - pos);

            float NdotS = max(dot(n, sampleDir), 0);
            float VPdistSP = distance(pos, samplePos);

            float a = OneMinus(smoothstep(AO.threshold, AO.threshold * 2, VPdistSP));

            if(VPdistSP < AO.maxDist)
                occlusion += a * NdotS;
        }

        outColor.rgb = OneMinus(occlusion / length) * c * ambientColor;
        outColor.a = 1;
    } else {
        outColor = vec4(0, 0, 0, 0);
    }
}
