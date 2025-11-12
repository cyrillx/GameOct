#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTex;
in vec3 vTangent;

out vec4 FragColor;

// Light structures (std140 layout for UBO)
struct DirLight {
    vec3 direction;
    float pad0;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    float pad0;
    vec3 color;
    float intensity;
    float radius;
    float pad1, pad2, pad3;
};

struct SpotLight {
    vec3 position;
    float pad0;
    vec3 direction;
    float pad1;
    vec3 color;
    float intensity;
    float innerCone;
    float outerCone;
    float pad2, pad3;
};

layout(std140) uniform Lights {
    DirLight dirLights[4];
    PointLight pointLights[4];
    SpotLight spotLights[4];
    mat4 dirLightMatrices[4];
    mat4 spotLightMatrices[4];
};

uniform sampler2D uDiffuse;
uniform sampler2D uSpecular;
uniform sampler2D uNormalMap;

uniform sampler2D dirShadowMaps[4];
uniform sampler2D spotShadowMaps[4];
uniform samplerCube pointShadowCubes[4];

uniform vec3 viewPos;

// Fog parameters
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform vec2 fogHeightZone; // (minY, maxY)

// Compute normal from normal map using TBN matrix
vec3 getNormalFromMap() {
    vec3 N = normalize(vNormal);
    vec3 T = normalize(vTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    vec3 texNormal = texture(uNormalMap, vTex).rgb;
    texNormal = normalize(texNormal * 2.0 - 1.0);
    
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * texNormal);
}

// PCF shadow sampling for directional light
float sampleDirShadowPCF(int idx, vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
       projCoords.y < 0.0 || projCoords.y > 1.0) return 0.0;
    
    float shadow = 0.0;
    float bias = 0.005;
    vec2 texelSize = 1.0 / textureSize(dirShadowMaps[idx], 0);
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(dirShadowMaps[idx], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth ? 1.0 : 0.0);
        }
    }
    shadow /= 9.0;
    return shadow;
}

// PCF shadow sampling for spot light (similar to directional)
float sampleSpotShadowPCF(int idx, vec4 fragPosLightSpace) {
    return sampleDirShadowPCF(idx, fragPosLightSpace); // same PCF logic
}

// Shadow sampling for point light using cubemap
float samplePointShadow(int idx, vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias = 0.15;
    float samples = 4.0;
    float offset = 0.1;
    
    for(float x = -offset; x < offset; x += offset / 2.0) {
        for(float y = -offset; y < offset; y += offset / 2.0) {
            for(float z = -offset; z < offset; z += offset / 2.0) {
                float pcfDepth = texture(pointShadowCubes[idx], fragToLight + vec3(x,y,z)).r;
                pcfDepth *= 25.0; // normalize cubemap depth to 0..1 (rough approximation)
                shadow += (currentDepth - bias > pcfDepth ? 1.0 : 0.0);
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

void main() {
    vec3 albedo = texture(uDiffuse, vTex).rgb;
    vec3 specMap = texture(uSpecular, vTex).rgb;
    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - vWorldPos);
    
    vec3 result = vec3(0.0);
    
    // Directional lights
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(-dirLights[i].direction);
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        
        vec4 fragPosLightSpace = dirLightMatrices[i] * vec4(vWorldPos, 1.0);
        float shadow = sampleDirShadowPCF(i, fragPosLightSpace);
        
        result += (1.0 - shadow) * dirLights[i].color * dirLights[i].intensity * (albedo * diff + spec);
    }
    
    // Point lights
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(pointLights[i].position - vWorldPos);
        float dist = length(pointLights[i].position - vWorldPos);
        float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        
        float shadow = samplePointShadow(i, vWorldPos, pointLights[i].position);
        
        result += (1.0 - shadow) * pointLights[i].color * pointLights[i].intensity * attenuation * (albedo * diff + spec);
    }
    
    // Spot lights
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(spotLights[i].position - vWorldPos);
        float theta = dot(L, normalize(-spotLights[i].direction));
        float epsilon = spotLights[i].innerCone - spotLights[i].outerCone;
        float intensity = clamp((theta - spotLights[i].outerCone) / epsilon, 0.0, 1.0);
        
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        
        vec4 fragPosLightSpace = spotLightMatrices[i] * vec4(vWorldPos, 1.0);
        float shadow = sampleSpotShadowPCF(i, fragPosLightSpace);
        
        result += (1.0 - shadow) * spotLights[i].color * spotLights[i].intensity * intensity * (albedo * diff + spec);
    }
    
    // Simple ambient
    result += vec3(0.03) * albedo;
    
    // Volumetric fog: distance-based with height zone
    float distance = length(viewPos - vWorldPos);
    float fogFactor = clamp((distance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    float heightFactor = smoothstep(fogHeightZone.x, fogHeightZone.y, vWorldPos.y);
    float finalFog = mix(0.0, fogFactor, heightFactor);
    
    vec3 color = mix(result, fogColor, finalFog);
    
    FragColor = vec4(color, 1.0);
}
