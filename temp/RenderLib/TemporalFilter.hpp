#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <cstdint>

namespace RenderLib {

class Shader;

/**
 * @brief Post-process temporal filtering (TAA-like without full complexity)
 * 
 * Smooths temporal artifacts and reduces flicker using motion-compensated
 * frame history blending.
 * 
 * Performance: +1-2ms, but significantly improves visual stability
 */
class TemporalFilter {
public:
    struct Config {
        float feedbackMin = 0.85f;        // Min history retention
        float feedbackMax = 0.95f;        // Max history retention
        float velocityThreshold = 5.0f;   // Pixels/frame (motion detection)
        bool useMotionVectors = true;     // Use motion info for projection
    };

    TemporalFilter();
    ~TemporalFilter();

    bool init(uint32_t width, uint32_t height, const Config& config = Config{});
    void destroy();

    // Main call: filter current frame using history
    // Returns filtered result texture
    GLuint filter(GLuint currentFrame, GLuint velocityTexture = 0);

    // Update history for next frame
    void swapHistory();

    // Get current filtered result
    GLuint getFilteredTexture() const { return filteredTexture_; }

    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

private:
    Config config_;
    uint32_t width_ = 1920;
    uint32_t height_ = 1080;

    // Frame history
    GLuint currentFrameTexture_ = 0;
    GLuint historyFrameTexture_ = 0;
    GLuint filteredTexture_ = 0;

    // FBOs
    GLuint temporalFBO_ = 0;

    // Shader
    std::shared_ptr<Shader> temporalFilterShader_;

    bool createFramebuffers_(uint32_t w, uint32_t h);
};

}  // namespace RenderLib
