#version 330 core

in vec3 vWorldPos;
out vec4 FragColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform float opacity;

void main() {
    // Project position to [0,1] for UV mapping
    vec3 pos = fract(vWorldPos);
    vec2 uv = pos.xy;
    
    vec3 albedo = texture(albedoTex, uv).rgb;
    vec3 normal = texture(normalTex, uv).rgb * 2.0 - 1.0;
    
    // Check if we're on a front-facing surface
    float alpha = texture(albedoTex, uv).a;
    if (alpha < 0.5) discard;
    
    FragColor = vec4(albedo, opacity * alpha);
}
