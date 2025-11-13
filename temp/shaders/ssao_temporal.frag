#version 330 core

in vec2 vTex;
out vec4 FragColor;

uniform sampler2D depthTexture;
uniform int sampleCount;
uniform float radius;
uniform float bias;

uniform mat4 projection;
uniform mat4 invProjection;

// Sample pattern: directions on hemisphere
const vec3 sampleDirections[16] = vec3[](
    vec3(1, 1, 1), vec3(-1, -1, -1), vec3(1, -1, 1), vec3(-1, 1, -1),
    vec3(1, 1, -1), vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, -1, -1),
    vec3(1, 0, 1), vec3(-1, 0, -1), vec3(0, 1, 1), vec3(0, -1, -1),
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(1, -1, 0), vec3(-1, 1, 0)
);

float getDepth(vec2 uv) {
    return texture(depthTexture, uv).r;
}

vec3 getWorldPos(vec2 uv, float depth) {
    vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 worldPos = invProjection * ndc;
    return worldPos.xyz / worldPos.w;
}

void main() {
    float depth = getDepth(vTex);
    vec3 pos = getWorldPos(vTex, depth);
    vec3 normal = normalize(cross(dFdx(pos), dFdy(pos)));
    
    float occlusion = 0.0;
    
    for (int i = 0; i < sampleCount; ++i) {
        // Sample sphere around position
        vec3 samplePos = pos + sampleDirections[i] * radius;
        
        // Project to screen
        vec4 proj = projection * vec4(samplePos, 1.0);
        proj /= proj.w;
        vec2 sampleUV = proj.xy * 0.5 + 0.5;
        
        // Check if occluded
        float sampleDepth = getDepth(sampleUV);
        vec3 sampleWorldPos = getWorldPos(sampleUV, sampleDepth);
        
        float dist = length(sampleWorldPos - pos);
        float rangeCheck = smoothstep(radius * 2.0, 0.0, dist);
        
        if (sampleWorldPos.y < pos.y - bias) {
            occlusion += rangeCheck;
        }
    }
    
    float ssao = 1.0 - (occlusion / float(sampleCount));
    FragColor = vec4(vec3(ssao), 1.0);
}
