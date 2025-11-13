#version 330 core
in vec3 vPos;
out vec4 FragColor;

void main() {
    vec3 base = vec3(0.8, 0.7, 0.6);
    vec3 lightDir = normalize(vec3(0.3, 0.7, 0.2));
    float shade = clamp(dot(normalize(vPos), lightDir) * 0.5 + 0.5, 0.0, 1.0);
    FragColor = vec4(base * shade, 1.0);
}
