#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;
class Texture2D;

/**
 * @brief Volumetric lighting system (god rays effect)
 * 
 * Renders visible light rays through atmosphere using ray marching.
 * Compatible with forward rendering, applies as post-process.
 * 
 * Performance: 3-5ms at 1080p (depending on ray steps and sampling)
 */
class VolumetricLighting {
public:
    struct Config {
        uint32_t raySteps = 16;           // 8-32 samples along ray
        float stepSize = 1.0f;            // World units per step (0.5-2.0)
        float scatteringStrength = 0.8f;  // Intensity multiplier (0.0-1.0)
        float rayStartDistance = 10.0f;   // Distance where rays begin (0-100m)
        float heightAbsorption = 0.1f;    // Height-based fog absorption (0.0-1.0)
        bool useBlueNoise = true;         // Dithering for smoother results
        glm::vec3 sunColor = {1.0f, 0.95f, 0.8f};
    };

    VolumetricLighting();
    ~VolumetricLighting();

    bool init(const Config& config = Config{});
    void destroy();

    // Update parameters
    void setConfig(const Config& config) { config_ = config; }
    void setRaySteps(uint32_t steps);
    void setScatteringStrength(float strength);
    void setStepSize(float size);

    // Rendering
    // Requires depth texture + color texture as input
    // Returns volumetric lighting texture
    GLuint render(GLuint inputDepthTexture, GLuint inputColorTexture,
                  const glm::mat4& projection, const glm::mat4& invProjection,
                  const glm::vec3& cameraPos, const glm::vec3& sunDir);

    // Apply volumetric to final image
    void applyToImage(GLuint sceneTexture, GLuint volumetricTexture, 
                     float blendStrength = 0.8f);

    // Get current framebuffer texture
    GLuint getVolumetricTexture() const { return volumetricTexture_; }

    Config getConfig() const { return config_; }

private:
    Config config_;

    // Framebuffer for volumetric pass
    GLuint volumetricFBO_ = 0;
    GLuint volumetricTexture_ = 0;  // RGBA16F
    GLuint volumetricDepth_ = 0;    // RBO

    // Shaders
    std::shared_ptr<Shader> volumetricShader_;  // ray marching
    std::shared_ptr<Shader> blendShader_;       // apply to scene

    // Blue noise for dithering
    std::shared_ptr<Texture2D> blueNoiseTexture_;

    uint32_t width_ = 1920;
    uint32_t height_ = 1080;

    bool createFramebuffer_(uint32_t w, uint32_t h);
    void loadBlueNoise_();
};

}  // namespace RenderLib
