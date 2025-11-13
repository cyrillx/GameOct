#include "RenderLib/WindSystem.hpp"
#include "RenderLib/Shader.hpp"
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

namespace RenderLib {

WindSystem::WindSystem() {}

WindSystem::~WindSystem() {
    destroy();
}

bool WindSystem::init(const Config& config) {
    config_ = config;

    // Create wind compute shader
    windComputeShader_ = std::make_shared<Shader>();
    if (!windComputeShader_->compileCompute("wind_compute.comp")) {
        std::cerr << "Failed to compile wind compute shader\n";
        return false;
    }

    // Create 3D noise texture
    if (!createNoiseTexture_()) {
        std::cerr << "Failed to create wind noise texture\n";
        return false;
    }

    return true;
}

void WindSystem::destroy() {
    if (windNoiseTexture_) {
        glDeleteTextures(1, &windNoiseTexture_);
        windNoiseTexture_ = 0;
    }
}

bool WindSystem::createNoiseTexture_() {
    uint32_t res = config_.noiseResolution;
    
    // Generate 3D Perlin noise
    std::vector<glm::vec4> noiseData(res * res * res);
    
    for (uint32_t z = 0; z < res; ++z) {
        for (uint32_t y = 0; y < res; ++y) {
            for (uint32_t x = 0; x < res; ++x) {
                glm::vec3 pos = glm::vec3(x, y, z) / glm::vec3(res);
                
                // Octave Perlin noise
                float noise = glm::perlin(pos * 4.0f) * 0.5f +
                             glm::perlin(pos * 8.0f) * 0.25f +
                             glm::perlin(pos * 16.0f) * 0.125f;
                
                noise = glm::clamp(noise * 0.5f + 0.5f, 0.0f, 1.0f);
                
                // Store as wind direction (XYZ) + strength (W)
                glm::vec3 dir = glm::normalize(glm::vec3(
                    glm::perlin(pos + glm::vec3(0.5f)),
                    glm::perlin(pos + glm::vec3(1.0f)),
                    glm::perlin(pos + glm::vec3(1.5f))
                ));
                
                noiseData[z * res * res + y * res + x] = 
                    glm::vec4(dir * 0.5f + 0.5f, noise);
            }
        }
    }

    glGenTextures(1, &windNoiseTexture_);
    glBindTexture(GL_TEXTURE_3D, windNoiseTexture_);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, res, res, res, 0, 
                 GL_RGBA, GL_FLOAT, noiseData.data());
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glBindTexture(GL_TEXTURE_3D, 0);
    return true;
}

void WindSystem::computeWindField_() {
    if (!windComputeShader_) return;

    uint32_t res = config_.noiseResolution;
    
    windComputeShader_->use();
    windComputeShader_->setFloat("time", timePhase_);
    windComputeShader_->setVec3("baseDir", config_.baseDirection);
    windComputeShader_->setFloat("strength", config_.baseStrength);
    windComputeShader_->setFloat("turbulence", config_.turbulence);

    glBindImageTexture(0, windNoiseTexture_, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    glDispatchCompute((res + 7) / 8, (res + 7) / 8, (res + 7) / 8);
    glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
}

void WindSystem::update(float deltaTime) {
    timePhase_ += deltaTime;
    updateCounter_++;

    // Recompute wind every 4 frames
    if (!config_.updateEveryFrame && updateCounter_ % 4 == 0) {
        computeWindField_();
    } else if (config_.updateEveryFrame) {
        computeWindField_();
    }
}

glm::vec3 WindSystem::getWindAt(const glm::vec3& worldPos, float heightInfluence) {
    // Base wind from config
    glm::vec3 wind = config_.baseDirection * config_.baseStrength;
    
    // Add oscillation
    float oscillation = std::sin(timePhase_ * config_.frequency);
    wind += glm::vec3(
        std::sin(timePhase_ * config_.frequency + worldPos.x * 0.1f),
        0.0f,
        std::cos(timePhase_ * config_.frequency + worldPos.z * 0.1f)
    ) * config_.turbulence * 0.5f;
    
    // Height influence: stronger wind at higher altitudes
    float heightFactor = 1.0f + (worldPos.y / 100.0f) * heightInfluence * 0.5f;
    wind *= heightFactor;
    
    return wind;
}

}  // namespace RenderLib
