#version 330 core

in VS_OUT {
    vec3 vWorldPos;
    vec3 vNormal;
    vec2 vTex;
} fs_in;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uTreeTexture;
uniform vec3 uCameraPos;

void main() {
    vec4 texColor = texture(uTreeTexture, fs_in.vTex);

    if (texColor.a < 0.5) discard;

    vec3 treeColor = texColor.rgb;

    // Simple diffuse lighting
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    float NdotL = max(dot(fs_in.vNormal, lightDir), 0.3);
    treeColor *= NdotL;

    // Distance fog
    float dist = distance(fs_in.vWorldPos, uCameraPos);
    float fogFactor = exp(-0.005 * dist);
    vec3 fogColor = vec3(0.7, 0.8, 1.0);
    treeColor = mix(fogColor, treeColor, fogFactor);

    FragColor = vec4(treeColor, 1.0);
}
