#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;

/**
 * @brief Bloom/Glow post-processing effect
 * 
 * Creates glowing halos around bright objects.
 * Algorithm:
 * 1. Extract bright pixels (threshold-based)
 * 2. Blur them with progressively larger kernels (Gaussian)
 * 3. Blend back with original image
 */
class BloomPostProcess {
public:
    struct Config {
        float threshold = 1.0f;         // Brightness threshold
        float intensity = 1.0f;         // Glow intensity multiplier
        int iterations = 5;             // Mipmap levels for blur
        float curve = 2.0f;             // Curve (higher = sharper falloff)
        float maxBrightness = 10.0f;    // Maximum sampled brightness
        bool useAdaptiveBrightness = false;  // Adapt threshold to scene
    };

    BloomPostProcess();
    ~BloomPostProcess();

    bool init(int width, int height);
    void resize(int width, int height);
    void destroy();

    /**
     * Render bloom effect
     * @param colorTex - Input HDR color texture
     */
    void render(GLuint colorTex);

    /**
     * Get bloom texture (can be used for other effects or compositing)
     */
    GLuint getBloomTexture() const { return bloomTex_; }

    /**
     * Get intermediate bright extraction texture
     */
    GLuint getBrightTexture() const { return brightTex_; }

    /**
     * Configure bloom
     */
    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

private:
    void createFramebuffers_();
    void extractBright_();
    void blurHierarchy_();
    void blur_(GLuint input, GLuint output, bool horizontal);

    GLuint brightFBO_ = 0;
    GLuint bloomFBO_ = 0;

    GLuint brightTex_ = 0;
    GLuint bloomTex_ = 0;
    GLuint blurTex_[2] = {0, 0};  // Ping-pong for blur

    std::shared_ptr<Shader> extractShader_;
    std::shared_ptr<Shader> blurShader_;

    Config config_;
    int width_ = 0;
    int height_ = 0;
    GLuint quadVAO_ = 0;
};

} // namespace RenderLib
