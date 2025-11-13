#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 4) in vec4 iMat0;
layout(location = 5) in vec4 iMat1;
layout(location = 6) in vec4 iMat2;
layout(location = 7) in vec4 iMat3;

uniform mat4 uViewProj;

out vec3 vPos;

mat4 instanceMat() {
    return mat4(iMat0, iMat1, iMat2, iMat3);
}

void main() {
    mat4 model = instanceMat();
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = uViewProj * worldPos;
    vPos = worldPos.xyz;
}
