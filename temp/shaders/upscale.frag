#version 330 core

in vec2 vTex;
out vec4 FragColor;

uniform sampler2D inputTexture;
uniform vec2 inputSize;
uniform vec2 outputSize;

// FSR-like upscaling with edge enhancement
void main() {
    vec2 uv = vTex;
    
    // Compute texel size in source coordinates
    vec2 srcTexelSize = 1.0 / inputSize;
    vec2 dstTexelSize = 1.0 / outputSize;
    
    // Map output UV to input space
    vec2 srcUV = uv * (inputSize / outputSize);
    
    // Gather 4 samples for interpolation
    vec2 f = fract(srcUV);
    vec2 p0 = floor(srcUV) * srcTexelSize;
    
    vec3 s00 = texture(inputTexture, p0).rgb;
    vec3 s10 = texture(inputTexture, p0 + vec2(srcTexelSize.x, 0)).rgb;
    vec3 s01 = texture(inputTexture, p0 + vec2(0, srcTexelSize.y)).rgb;
    vec3 s11 = texture(inputTexture, p0 + srcTexelSize).rgb;
    
    // Bicubic interpolation
    vec3 s0 = mix(s00, s10, f.x);
    vec3 s1 = mix(s01, s11, f.x);
    vec3 color = mix(s0, s1, f.y);
    
    // Edge detection for sharpening
    float edge = length(s00 - s11) + length(s10 - s01);
    color *= (1.0 + edge * 0.1);  // Slight sharpening
    
    FragColor = vec4(color, 1.0);
}
