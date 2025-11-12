#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D sceneTex;

// Very simple and cheap SSR approximation: sample along reflected vector in screen-space
void main(){
    vec3 pos = texture(gPosition, TexCoords).xyz;
    vec3 N = normalize(texture(gNormal, TexCoords).xyz);
    vec3 V = normalize(-pos);
    vec3 R = reflect(V, N);
    vec2 uv = TexCoords;
    vec3 accum = vec3(0.0);
    float stride = 0.02;
    for(int i=1;i<=20;i++){
        uv += R.xy * stride;
        vec3 sample = texture(sceneTex, uv).rgb;
        accum += sample * (1.0/float(i));
    }
    accum /= 20.0;
    FragColor = vec4(accum, 1.0);
}
