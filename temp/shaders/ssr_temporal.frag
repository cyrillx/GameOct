#version 330 core

// SSR Temporal Reprojection
layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uCurrent;
uniform sampler2D uPrevious;
uniform float uBlendFactor;

void main() {
    vec4 current = texture(uCurrent, TexCoord);
    vec4 previous = texture(uPrevious, TexCoord);
    
    // Temporal blend
    FragColor = mix(current, previous, uBlendFactor);
}
