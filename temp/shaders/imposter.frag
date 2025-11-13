#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTex;

out vec4 FragColor;

uniform sampler2DArray imposterTextures;  // Albedo + normal in different layers
uniform uint angleIndex;

void main() {
    // Sample imposter at correct angle
    vec3 sampleCoord = vec3(vTex, angleIndex);
    
    vec4 imposter = texture(imposterTextures, sampleCoord);
    
    if (imposter.a < 0.1) discard;  // Alpha test
    
    FragColor = imposter;
}
