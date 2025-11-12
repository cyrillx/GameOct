#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D sceneTex;
uniform sampler2D bloomTex;

void main(){
    vec3 hdr = texture(sceneTex, TexCoords).rgb;
    vec3 bloom = texture(bloomTex, TexCoords).rgb;
    vec3 result = hdr + bloom;
    // simple Reinhard tonemapping
    result = result / (result + vec3(1.0));
    FragColor = vec4(result, 1.0);
}
