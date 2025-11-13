#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform float uTime;
uniform float uWindStrength;
uniform float uWindFrequency;

out VS_OUT {
    vec3 vWorldPos;
    vec2 vTex;
    float vHeight;
} vs_out;

void main() {
    vec3 worldPos = (uModel * vec4(aPosition, 1.0)).xyz;

    // Wind deformation: sway based on height and time
    float windPhase = uTime * uWindFrequency;
    float sway = sin(windPhase + worldPos.x * 0.1 + worldPos.z * 0.1) * uWindStrength;
    
    // Apply wind primarily to upper vertices
    float heightFactor = max(0.0, aPosition.y / 2.0);  // Normalize by typical grass height
    worldPos.x += sway * heightFactor * 0.3;
    worldPos.z += cos(windPhase) * uWindStrength * heightFactor * 0.2;

    vs_out.vWorldPos = worldPos;
    vs_out.vTex = aTexCoord;
    vs_out.vHeight = aPosition.y;

    gl_Position = uProjection * uView * vec4(worldPos, 1.0);
}
