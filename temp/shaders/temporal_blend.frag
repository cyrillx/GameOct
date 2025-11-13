#version 330 core

in vec2 vTex;
out vec4 FragColor;

uniform sampler2D currentSSAO;
uniform sampler2D historySSAO;
uniform float temporalWeight;

void main() {
    float current = texture(currentSSAO, vTex).r;
    float history = texture(historySSAO, vTex).r;
    
    // Simple temporal blend with clamping to avoid ghosting
    float temporalSSAO = mix(current, history, temporalWeight);
    
    // Clamp to smooth out extreme differences
    temporalSSAO = clamp(temporalSSAO, 
                        min(current, history) - 0.1,
                        max(current, history) + 0.1);
    
    FragColor = vec4(vec3(temporalSSAO), 1.0);
}
