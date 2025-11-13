#version 330 core

// Color Grading via 3D LUT
layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uColorTex;
uniform sampler3D uLUT;

uniform float uIntensity;
uniform float uContrast;
uniform float uSaturation;
uniform float uBrightness;
uniform float uLUTSize;

vec3 applyLUT(vec3 color) {
    // Scale to LUT coordinates
    vec3 lutCoord = color * (uLUTSize - 1.0) / uLUTSize + 0.5 / uLUTSize;
    return texture(uLUT, lutCoord).rgb;
}

void main() {
    vec3 color = texture(uColorTex, TexCoord).rgb;
    
    // Apply brightness
    color = color + uBrightness;
    
    // Apply contrast
    color = (color - 0.5) * uContrast + 0.5;
    
    // Apply saturation
    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(vec3(gray), color, uSaturation);
    
    // Apply LUT
    vec3 graded = applyLUT(color);
    
    // Blend with original
    color = mix(color, graded, uIntensity);
    
    FragColor = vec4(color, 1.0);
}
