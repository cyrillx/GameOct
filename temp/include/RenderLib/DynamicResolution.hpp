#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <memory>

namespace RenderLib {

class Shader;

/**
 * @brief Dynamic resolution scaling based on frame time
 * 
 * Automatically adjusts render resolution to maintain target FPS.
 * Upscales lower resolution to full size using FSR-like technique.
 * 
 * Performance: Adaptive - maintains consistent FPS across hardware
 */
class DynamicResolution {
public:
    struct Config {
        float targetFPS = 60.0f;          // Target frame rate
        float minResolutionScale = 0.5f;  // Minimum 50% resolution
        float maxResolutionScale = 1.0f;  // Maximum 100% resolution
        float scaleStep = 0.05f;          // Adjustment step size (5% per frame)
        uint32_t stabilizeFrames = 10;    // Frames to wait before adjusting
    };

    DynamicResolution();
    ~DynamicResolution();

    bool init(uint32_t baseWidth, uint32_t baseHeight, const Config& config = Config{});
    void destroy();

    // Update resolution based on frame time (call after rendering)
    void update(float gpuTime, float cpuTime);

    // Get current resolution scale
    float getResolutionScale() const { return currentScale_; }

    // Get render target dimensions
    glm::vec2 getRenderDimensions() const;

    // Get full screen dimensions
    glm::vec2 getDisplayDimensions() const;

    // Upscale low-res to full resolution
    void upscaleToDisplay(GLuint lowResTexture, GLuint displayFramebuffer);

    Config getConfig() const { return config_; }
    void setConfig(const Config& config) { config_ = config; }

    // Statistics
    struct Statistics {
        float avgGPUTime = 0.0f;
        float avgCPUTime = 0.0f;
        float currentScale = 1.0f;
        uint32_t scaleAdjustments = 0;
    };
    Statistics getStatistics() const { return stats_; }

private:
    Config config_;
    Statistics stats_;

    uint32_t baseWidth_ = 1920;
    uint32_t baseHeight_ = 1080;

    float currentScale_ = 1.0f;
    float targetFrameTime_ = 0.0f;  // 1/targetFPS

    uint32_t stabilizeCounter_ = 0;
    float avgGPUTime_ = 0.0f;
    float avgCPUTime_ = 0.0f;

    // Upscale shader
    std::shared_ptr<Shader> upscaleShader_;

    GLuint upscaleFBO_ = 0;

    bool createUpscaleResources_();
};

}  // namespace RenderLib
