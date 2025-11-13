#version 330 core

in vec2 vTex;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D volumetricTexture;
uniform float blendStrength;

void main() {
    vec3 scene = texture(sceneTexture, vTex).rgb;
    vec3 volumetric = texture(volumetricTexture, vTex).rgb;
    
    // Additive blending for light rays
    vec3 result = scene + volumetric * blendStrength;
    
    FragColor = vec4(result, 1.0);
}
