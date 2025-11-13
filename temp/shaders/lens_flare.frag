#version 330 core

// Lens Flare Effect
layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform vec2 uSunScreenPos;
uniform vec3 uSunColor;
uniform float uIntensity;

const int FLARE_COUNT = 8;
const float FLARE_OFFSETS[8] = float[](
    0.1, 0.2, 0.35, 0.5, 0.7, 0.85, 0.95, 1.1
);

void main() {
    vec2 towardsSun = normalize(uSunScreenPos - TexCoord);
    vec2 distFromSun = uSunScreenPos - TexCoord;
    float distToSun = length(distFromSun);
    
    vec3 flare = vec3(0.0);
    
    // Multiple lens flare elements
    for (int i = 0; i < FLARE_COUNT; i = i + 1) {
        float offset = FLARE_OFFSETS[i];
        vec2 flarePos = uSunScreenPos - towardsSun * distToSun * offset;
        
        float dist = length(flarePos - TexCoord);
        float size = 0.05 * (1.0 - offset);
        float intensity = smoothstep(size, 0.0, dist);
        
        flare = flare + uSunColor * intensity * (1.0 - offset);
    }
    
    FragColor = vec4(flare * uIntensity, 1.0);
}
