#version 330 core

// Vertex shader for GPU culling transform-feedback pipeline
layout(location = 0) in vec3 aPosition; // a point representing the instance bounding center
// Instance attributes: model matrix passed via divisor=1 (we assume bound by caller)
layout(location = 4) in vec4 iMat0;
layout(location = 5) in vec4 iMat1;
layout(location = 6) in vec4 iMat2;
layout(location = 7) in vec4 iMat3;

out mat4 vModel;

void main(){
    vModel[0] = iMat0;
    vModel[1] = iMat1;
    vModel[2] = iMat2;
    vModel[3] = iMat3;
    // pass through to geometry shader
}
