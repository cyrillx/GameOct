#version 330 core

// Edge Composite - Blend edges with original color
layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uColorTex;
uniform sampler2D uEdgeTex;
uniform float uBlendAmount;

void main() {
    vec4 color = texture(uColorTex, TexCoord);
    vec4 edges = texture(uEdgeTex, TexCoord);
    
    // Composite edges over color
    vec3 result = mix(color.rgb, edges.rgb, edges.a * uBlendAmount);
    
    FragColor = vec4(result, color.a);
}
