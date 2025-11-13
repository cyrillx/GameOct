#version 330 core

in VS_OUT {
    vec3 vWorldPos;
    vec3 vNormal;
    vec2 vTex;
} fs_in;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uNormalMap1;
uniform sampler2D uNormalMap2;
uniform sampler2D uReflectionTex;
uniform sampler2D uDepthTex;

uniform vec2 uNormalOffset;
uniform vec3 uShallowColor;
uniform vec3 uDeepColor;
uniform float uDepthScale;
uniform float uFresnelPower;
uniform vec3 uCameraPos;

void main() {
    // Sample two layers of normal maps with scrolling offset
    vec2 uv1 = fs_in.vTex + uNormalOffset;
    vec2 uv2 = fs_in.vTex - uNormalOffset * 0.7;

    vec3 normal1 = normalize(texture(uNormalMap1, uv1).rgb * 2.0 - 1.0);
    vec3 normal2 = normalize(texture(uNormalMap2, uv2).rgb * 2.0 - 1.0);
    vec3 normal = normalize(normal1 + normal2);

    // Blend with surface normal
    normal = normalize(normal * 0.8 + fs_in.vNormal * 0.2);

    // Fresnel effect
    vec3 viewDir = normalize(uCameraPos - fs_in.vWorldPos);
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), uFresnelPower);

    // Reflection
    vec3 reflection = texture(uReflectionTex, fs_in.vTex).rgb;

    // Depth-based water color
    float depth = texture(uDepthTex, fs_in.vTex).r;
    float depthFactor = clamp(depth / uDepthScale, 0.0, 1.0);
    vec3 waterColor = mix(uShallowColor, uDeepColor, depthFactor);

    // Combine
    vec3 finalColor = mix(waterColor, reflection, fresnel);
    finalColor += vec3(0.1) * max(0.0, dot(normal, -normalize(vec3(1.0, 1.0, 1.0))));  // Simple specular

    FragColor = vec4(finalColor, 0.7);
}
