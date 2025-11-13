#version 330 core

layout(points) in;
layout(points, max_vertices = 1) out;

in mat4 vModel[];

uniform mat4 uViewProj;

// Output captured per-instance matrix as 4 vec4 varyings for transform feedback
out vec4 tfMat0;
out vec4 tfMat1;
out vec4 tfMat2;
out vec4 tfMat3;

void main(){
    // Compute bounding sphere center in world-space from model matrix
    vec3 center = vec3(vModel[0][3], vModel[1][3], vModel[2][3]);
    // Simple frustum cull: transform center to clip and compare - this is an approximation
    vec4 clip = uViewProj * vec4(center, 1.0);
    bool visible = true;
    if (clip.w == 0.0) visible = false;
    vec3 ndc = clip.xyz / clip.w;
    if (ndc.x < -1.1 || ndc.x > 1.1 || ndc.y < -1.1 || ndc.y > 1.1 || ndc.z < -1.0 || ndc.z > 1.0) visible = false;

    if (visible) {
        tfMat0 = vModel[0];
        tfMat1 = vModel[1];
        tfMat2 = vModel[2];
        tfMat3 = vModel[3];
        EmitVertex();
        EndPrimitive();
    }
}
