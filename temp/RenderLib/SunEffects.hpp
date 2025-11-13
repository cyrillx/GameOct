#pragma once
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;

/**
 * @brief Sun Effects: Lens Flare, God Rays, Light Shafts
 * 
 * Simulates sun-based effects for atmospheric visual enhancement.
 */
class SunEffects {
public:
    struct Config {
        // God Rays / Light Shafts
        bool enableGodRays = true;
        float godRayIntensity = 1.0f;
        int godRaySamples = 32;         // Samples for ray marching
        float godRayDecay = 0.97f;      // Decay factor
        float godRayExposure = 1.0f;    // Post-processing exposure
        
        // Lens Flare
        bool enableLensFlare = true;
        float lensFlareIntensity = 0.5f;
        
        // Sun position and color
        glm::vec3 sunDirection = glm::normalize(glm::vec3(1, 1, 0));
        glm::vec3 sunColor = glm::vec3(1.0f, 0.95f, 0.8f);
    };

    SunEffects();
    ~SunEffects();

    bool init(int width, int height);
    void resize(int width, int height);
    void destroy();

    /**
     * Render god rays effect
     * @param colorTex - Scene color
     * @param depthTex - Scene depth (for occlusion)
     */
    void renderGodRays(GLuint colorTex, GLuint depthTex);

    /**
     * Render lens flare effect
     * @param screenPos - Sun position on screen [0,1]
     */
    void renderLensFlare(const glm::vec2& screenPos);

    /**
     * Get combined output texture
     */
    GLuint getOutput() const { return outputTex_; }

    /**
     * Configure effects
     */
    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

private:
    void createFramebuffers_();

    GLuint sunFBO_ = 0;
    GLuint outputTex_ = 0;
    GLuint godRayTex_ = 0;
    GLuint lensFlareTex_ = 0;
    
    std::shared_ptr<Shader> godRayShader_;
    std::shared_ptr<Shader> lensFlareShader_;

    Config config_;
    int width_ = 0;
    int height_ = 0;
    GLuint quadVAO_ = 0;
};

} // namespace RenderLib
