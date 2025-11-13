#version 330 core

in VS_OUT {
    vec3 vWorldPos;
    vec3 vNormal;
    vec2 vTex;
    vec3 vTangent;
} fs_in;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uDiffuse;
uniform sampler2D uNormal;
uniform vec3 uCameraPos;

// Lights (simplified: 1 directional)
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform float uLightIntensity;

// Fog
uniform float uFogDensity;
uniform float uFogHeight;
uniform vec3 uFogColor;

void main() {
    vec3 normal = normalize(fs_in.vNormal);

    // Sample normal map if available
    vec4 normalSample = texture(uNormal, fs_in.vTex);
    if (normalSample.a > 0.1) {
        vec3 mapNormal = normalize(normalSample.rgb * 2.0 - 1.0);
        vec3 bitangent = cross(normal, fs_in.vTangent);
        normal = normalize(fs_in.vTangent * mapNormal.x + bitangent * mapNormal.y + normal * mapNormal.z);
    }

    // Diffuse
    vec4 diffuseColor = texture(uDiffuse, fs_in.vTex);
    vec3 diffuse = diffuseColor.rgb;

    // Blinn-Phong
    vec3 lightDir = normalize(-uLightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 radiance = uLightColor * uLightIntensity * NdotL * diffuse;

    // Specular
    vec3 viewDir = normalize(uCameraPos - fs_in.vWorldPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfDir), 0.0);
    vec3 specular = uLightColor * pow(NdotH, 32.0) * 0.3;

    radiance += specular;

    // Height-based fog
    float heightDiff = fs_in.vWorldPos.y - uFogHeight;
    float fogFactor = exp(-uFogDensity * heightDiff);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(uFogColor, radiance, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
