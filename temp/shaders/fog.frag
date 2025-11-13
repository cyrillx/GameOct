#version 330 core

// Atmospheric Fog
layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uColorTex;
uniform sampler2D uDepthTex;

uniform vec3 uFogColor;
uniform float uFogDensity;
uniform float uFogFalloff;
uniform float uFogNear;
uniform float uFogFar;
uniform int uFogType;

uniform bool uUseHeightFog;
uniform float uHeightFogDensity;
uniform float uHeightFogFalloff;
uniform float uFogBaseHeight;

uniform float uSkyScattering;
uniform vec3 uSunDirection;
uniform vec3 uSunColor;
uniform mat4 uView;

float linearFog(float distance) {
    return 1.0 - clamp((uFogFar - distance) / (uFogFar - uFogNear), 0.0, 1.0);
}

float exponentialFog(float distance) {
    return 1.0 - exp(-uFogDensity * distance);
}

float exponentialSqFog(float distance) {
    float fog = uFogDensity * distance;
    return 1.0 - exp(-(fog * fog));
}

void main() {
    vec4 color = texture(uColorTex, TexCoord);
    float depth = texture(uDepthTex, TexCoord).r;
    
    // Simple linearization
    float distance = depth * (uFogFar - uFogNear) + uFogNear;
    
    float fogAmount = 0.0;
    if (uFogType == 0) {
        fogAmount = linearFog(distance);
    } else if (uFogType == 1) {
        fogAmount = exponentialFog(distance);
    } else {
        fogAmount = exponentialSqFog(distance);
    }
    
    // Height fog
    if (uUseHeightFog) {
        // Simple height-based adjustment
        fogAmount = mix(0.0, fogAmount, uHeightFogDensity);
    }
    
    // Sky scattering
    vec3 fogCol = uFogColor + uSunColor * uSkyScattering;
    
    FragColor = vec4(mix(color.rgb, fogCol, fogAmount), color.a);
}
