#version 330 core

in vec2 vTex;
out vec4 FragColor;

uniform sampler2D depthTexture;
uniform sampler2D colorTexture;
uniform sampler2D blueNoise;

uniform int raySteps;
uniform float stepSize;
uniform float scatteringStrength;
uniform float rayStartDistance;
uniform float heightAbsorption;

uniform vec3 cameraPos;
uniform vec3 sunDir;
uniform vec3 sunColor;

uniform mat4 invProjection;

// Reconstruct world position from depth
vec3 getWorldPos(vec2 uv, float depth) {
    vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 worldPos = invProjection * ndc;
    return worldPos.xyz / worldPos.w;
}

// Smooth step height-based absorption
float getHeightAbsorption(float height) {
    float normalized = clamp(height / 100.0, 0.0, 1.0);
    return exp(-normalized * heightAbsorption);
}

void main() {
    vec2 uv = vTex;
    float depth = texture(depthTexture, uv).r;
    
    // Get world position at this pixel
    vec3 worldPos = getWorldPos(uv, depth);
    vec3 rayDir = normalize(worldPos - cameraPos);
    
    // Start ray marching from camera
    vec3 rayPos = cameraPos;
    vec3 volumetric = vec3(0.0);
    
    // Dither step size for smoother results
    float dither = texture(blueNoise, uv * 4.0).r;
    float stepDistance = stepSize + dither * 0.5;
    
    // March along ray
    for (int step = 0; step < raySteps; ++step) {
        rayPos += rayDir * stepDistance;
        
        // Skip if behind camera or beyond surface
        if (length(rayPos - cameraPos) > length(worldPos - cameraPos)) {
            break;
        }
        
        // Height-based absorption (more fog at ground level)
        float heightFactor = getHeightAbsorption(rayPos.y);
        
        // Simple shadow check: project to light space
        // In production: sample shadow map at rayPos
        float lightVisibility = 1.0;  // TODO: sample shadow map
        
        // Accumulate scattering
        float transmittance = exp(-float(step) / float(raySteps) * 0.5);
        volumetric += lightVisibility * transmittance * heightFactor * sunColor;
    }
    
    // Normalize and apply strength
    volumetric *= scatteringStrength / float(raySteps);
    
    FragColor = vec4(volumetric, 1.0);
}
