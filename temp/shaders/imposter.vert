#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vWorldPos = (model * vec4(position, 1.0)).xyz;
    vNormal = normalize((model * vec4(normal, 0.0)).xyz);
    vTex = texCoord;
    
    gl_Position = projection * view * vec4(vWorldPos, 1.0);
}
