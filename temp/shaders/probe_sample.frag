#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform samplerCube uProbeCubemap;
uniform float uRoughness; // for simple prefiltered sampling

// Very simple sample: sample the cubemap along reflection direction
void main(){
    vec3 normal = vec3(0.0, 0.0, 1.0);
    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    vec3 refl = reflect(-viewDir, normal);
    vec3 col = texture(uProbeCubemap, refl).rgb;
    FragColor = vec4(col, 1.0);
}
