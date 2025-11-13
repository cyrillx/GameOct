#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;

/**
 * @brief Atmospheric Effects: Fog, Mist, Atmosphere
 * 
 * Simulates atmospheric scattering and depth-based fog.
 * Supports multiple fog types and height-based calculations.
 */
class AtmosphereRenderer {
public:
    enum class FogType {
        Linear,         // Fog = (far - dist) / (far - near)
        Exponential,    // Fog = exp(-c * dist)
        ExponentialSq   // Fog = exp(-c * dist^2)
    };

    struct Config {
        // Fog parameters
        glm::vec3 fogColor = glm::vec3(0.8f, 0.8f, 0.9f);
        float fogDensity = 0.01f;       // Fog density for exponential
        float fogFalloff = 1.0f;        // Distance multiplier
        float fogNear = 0.1f;           // Near fog distance
        float fogFar = 1000.0f;         // Far fog distance
        FogType fogType = FogType::Exponential;
        
        // Height fog (distance-independent)
        bool useHeightFog = true;
        float heightFogDensity = 0.1f;
        float heightFogFalloff = 0.5f;
        float fogBaseHeight = 0.0f;
        
        // Sky scattering (directional)
        float skyScattering = 0.5f;     // Rayleigh scattering strength
        glm::vec3 sunDirection = glm::normalize(glm::vec3(1, 1, 1));
        glm::vec3 sunColor = glm::vec3(1.0f);
        
        // Depth buffer usage
        bool useDepthBuffer = true;
    };

    AtmosphereRenderer();
    ~AtmosphereRenderer();

    bool init(int width, int height);
    void resize(int width, int height);
    void destroy();

    /**
     * Apply fog to scene
     * @param colorTex - Input scene color
     * @param depthTex - Depth texture (optional if useDepthBuffer = false)
     * @param view - View matrix for distance calculation
     */
    void applyFog(GLuint colorTex, GLuint depthTex, const glm::mat4& view);

    /**
     * Get fogged output texture
     */
    GLuint getOutput() const { return outputTex_; }

    /**
     * Configure atmosphere
     */
    void setConfig(const Config& config) { config_ = config; }
    Config getConfig() const { return config_; }

private:
    void createFramebuffers_();

    GLuint fogFBO_ = 0;
    GLuint outputTex_ = 0;
    std::shared_ptr<Shader> fogShader_;

    Config config_;
    int width_ = 0;
    int height_ = 0;
    GLuint quadVAO_ = 0;
};

} // namespace RenderLib
