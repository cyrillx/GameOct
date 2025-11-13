#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out VS_OUT {
    vec3 vWorldPos;
    vec3 vNormal;
    vec2 vTex;
} vs_out;

void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vs_out.vWorldPos = worldPos.xyz;
    vs_out.vNormal = normalize((uModel * vec4(aNormal, 0.0)).xyz);
    vs_out.vTex = aTexCoord;

    gl_Position = uProjection * uView * worldPos;
}
