#version 330 core

in VS_OUT {
    vec3 vWorldPos;
    vec2 vTex;
    float vHeight;
} fs_in;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uGrassTexture;
uniform vec3 uCameraPos;

void main() {
    vec4 texColor = texture(uGrassTexture, fs_in.vTex);

    // Alpha test
    if (texColor.a < 0.5) discard;

    // Simple grass coloring with height variation
    vec3 grassColor = texColor.rgb;
    
    // Darken at base, lighten at tip
    float heightFactor = fs_in.vHeight / 2.0;
    grassColor *= mix(0.6, 1.2, heightFactor);

    // Simple lighting
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    float NdotL = max(dot(normalize(fs_in.vWorldPos), lightDir), 0.3);
    grassColor *= NdotL;

    // Fog (simple distance-based)
    float dist = distance(fs_in.vWorldPos, uCameraPos);
    float fogFactor = exp(-0.01 * dist);
    vec3 fogColor = vec3(0.7, 0.8, 1.0);
    grassColor = mix(fogColor, grassColor, fogFactor);

    FragColor = vec4(grassColor, texColor.a);
}
