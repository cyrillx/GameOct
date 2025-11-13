#version 330 core

in vec2 vTex;
out vec4 FragColor;

uniform sampler2D currentFrame;
uniform sampler2D historyFrame;
uniform sampler2D velocityTexture;

uniform float feedbackMin;
uniform float feedbackMax;
uniform float velocityThreshold;
uniform bool useMotionVectors;

void main() {
    vec4 current = texture(currentFrame, vTex);
    vec4 history = texture(historyFrame, vTex);
    
    float feedback = mix(feedbackMin, feedbackMax, 0.5);
    
    // Motion-adaptive feedback
    if (useMotionVectors) {
        vec2 velocity = texture(velocityTexture, vTex).xy;
        float motionMagnitude = length(velocity);
        
        if (motionMagnitude > velocityThreshold) {
            // More motion = less history (avoid ghosting)
            feedback = feedbackMin;
        } else {
            // Less motion = more history (better stability)
            feedback = feedbackMax;
        }
    }
    
    // Temporal blend with clamping
    vec4 result = mix(current, history, feedback);
    
    // Clamp to prevent outliers
    result = clamp(result,
                  min(current, history) - 0.1,
                  max(current, history) + 0.1);
    
    FragColor = result;
}
