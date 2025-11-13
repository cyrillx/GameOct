#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <cstdint>

namespace RenderLib {

class Shader;
class Texture2D;

/**
 * @brief Advanced wind system with Perlin noise field
 * 
 * Replaces simple sin() wind with realistic turbulent wind.
 * Uses 3D noise texture computed in compute shader.
 * 
 * Performance: 1-2ms per frame (noise updated every 4 frames)
 */
class WindSystem {
public:
    struct Config {
        glm::vec3 baseDirection = {1.0f, 0.0f, 0.0f};
        float baseStrength = 0.5f;
        float frequency = 1.0f;
        float turbulence = 0.3f;  // Randomness factor
        
        uint32_t noiseResolution = 32;  // 32x32x32 3D texture
        bool updateEveryFrame = false;  // Or every 4 frames
    };

    WindSystem();
    ~WindSystem();

    bool init(const Config& config = Config{});
    void destroy();

    // Update wind field and parameters
    void update(float deltaTime);

    // Get wind at world position
    glm::vec3 getWindAt(const glm::vec3& worldPos, float heightInfluence = 1.0f);

    // Set parameters
    void setBaseDirection(const glm::vec3& dir) { config_.baseDirection = glm::normalize(dir); }
    void setBaseStrength(float strength) { config_.baseStrength = glm::clamp(strength, 0.0f, 2.0f); }
    void setFrequency(float freq) { config_.frequency = glm::clamp(freq, 0.1f, 5.0f); }
    void setTurbulence(float turb) { config_.turbulence = glm::clamp(turb, 0.0f, 1.0f); }

    // Get wind texture for shaders
    GLuint getWindNoiseTexture() const { return windNoiseTexture_; }
    
    Config getConfig() const { return config_; }

    float getTimePhase() const { return timePhase_; }

private:
    Config config_;

    // 3D texture for Perlin noise wind field
    GLuint windNoiseTexture_ = 0;

    // Compute shader for generating wind
    std::shared_ptr<Shader> windComputeShader_;

    float timePhase_ = 0.0f;
    uint32_t updateCounter_ = 0;

    bool createNoiseTexture_();
    void computeWindField_();
};

}  // namespace RenderLib
