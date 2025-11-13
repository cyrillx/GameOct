#version 330 core

in vec2 TexCoord;
layout(location = 0) out vec4 FragColor;

uniform sampler2D uColorTex;
uniform sampler2D uNormalTex;
uniform sampler2D uDepthTex;
uniform sampler2D uPositionTex;

uniform int uSampleCount;
uniform float uSampleRadius;
uniform float uFalloff;
uniform bool uUseBentNormals;

uniform mat4 uProj;
uniform mat4 uView;

// Simple random rotation based on UV
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 getViewPos(vec2 uv, float depth){
    float z = depth * 2.0 - 1.0;
    vec4 clip = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewPos = inverse(uProj) * clip;
    return viewPos.xyz / viewPos.w;
}

void main(){
    vec3 baseColor = texture(uColorTex, TexCoord).rgb;
    vec3 normal = normalize(texture(uNormalTex, TexCoord).rgb * 2.0 - 1.0);
    float depth = texture(uDepthTex, TexCoord).r;
    vec3 pos = texture(uPositionTex, TexCoord).rgb;

    // Approximate screen-space indirect diffuse by sampling nearby pixels
    vec3 indirect = vec3(0.0);
    float wsum = 0.0;
    float seed = rand(TexCoord);

    for (int i = 0; i < uSampleCount; ++i){
        float angle = (float(i) / float(uSampleCount)) * 6.283185 + seed;
        float radius = uSampleRadius * (0.2 + rand(TexCoord + float(i)*0.13));
        vec2 offset = vec2(cos(angle), sin(angle)) * radius / 1000.0; // small pixel offset
        vec2 sampleUV = TexCoord + offset;
        vec3 sNormal = normalize(texture(uNormalTex, sampleUV).rgb * 2.0 - 1.0);
        float sDepth = texture(uDepthTex, sampleUV).r;
        vec3 sPos = texture(uPositionTex, sampleUV).rgb;

        float dist = length(sPos - pos);
        float nl = max(dot(normal, normalize(sPos - pos)), 0.0);
        float weight = nl * exp(-dist * uFalloff);
        vec3 sColor = texture(uColorTex, sampleUV).rgb;
        indirect += sColor * weight;
        wsum += weight;
    }

    if (wsum > 0.0) indirect /= wsum;

    FragColor = vec4(indirect, 1.0);
}
