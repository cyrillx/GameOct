#version 330 core

// Edge Detection using Sobel/Roberts/Laplacian
layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uDepthTex;
uniform sampler2D uNormalTex;
uniform int uMethod;
uniform float uEdgeThreshold;
uniform float uEdgeStrength;
uniform vec3 uEdgeColor;
uniform bool uUseNormalEdges;
uniform bool uUseDepthEdges;
uniform float uDepthSensitivity;
uniform float uNormalSensitivity;
uniform bool uInvertEdges;

const vec2 offsets[8] = vec2[](
    vec2(-1, -1), vec2(0, -1), vec2(1, -1),
    vec2(-1,  0),              vec2(1,  0),
    vec2(-1,  1), vec2(0,  1), vec2(1,  1)
);

float sobelX(sampler2D tex) {
    float result = 0.0;
    result = result - texture(tex, TexCoord + offsets[0] * 0.001).r;
    result = result - texture(tex, TexCoord + offsets[3] * 0.001).r * 2.0;
    result = result - texture(tex, TexCoord + offsets[5] * 0.001).r;
    result = result + texture(tex, TexCoord + offsets[2] * 0.001).r;
    result = result + texture(tex, TexCoord + offsets[4] * 0.001).r * 2.0;
    result = result + texture(tex, TexCoord + offsets[7] * 0.001).r;
    return result;
}

float sobelY(sampler2D tex) {
    float result = 0.0;
    result = result - texture(tex, TexCoord + offsets[0] * 0.001).r;
    result = result - texture(tex, TexCoord + offsets[1] * 0.001).r * 2.0;
    result = result - texture(tex, TexCoord + offsets[2] * 0.001).r;
    result = result + texture(tex, TexCoord + offsets[5] * 0.001).r;
    result = result + texture(tex, TexCoord + offsets[6] * 0.001).r * 2.0;
    result = result + texture(tex, TexCoord + offsets[7] * 0.001).r;
    return result;
}

void main() {
    float edge = 0.0;
    
    // Depth edges
    if (uUseDepthEdges) {
        float sx = sobelX(uDepthTex);
        float sy = sobelY(uDepthTex);
        edge = sqrt(sx * sx + sy * sy) * uDepthSensitivity;
    }
    
    // Normal edges
    if (uUseNormalEdges) {
        float sx = sobelX(uNormalTex);
        float sy = sobelY(uNormalTex);
        float normalEdge = sqrt(sx * sx + sy * sy) * uNormalSensitivity;
        edge = max(edge, normalEdge);
    }
    
    // Threshold
    edge = step(uEdgeThreshold, edge);
    
    if (uInvertEdges) {
        edge = 1.0 - edge;
    }
    
    FragColor = vec4(vec3(edge) * uEdgeColor * uEdgeStrength, edge);
}
