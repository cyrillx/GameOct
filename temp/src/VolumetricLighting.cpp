#include "RenderLib/VolumetricLighting.hpp"
#include "RenderLib/Shader.hpp"
#include "RenderLib/Texture2D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace RenderLib {

VolumetricLighting::VolumetricLighting()
    : config_(Config{}) {}

VolumetricLighting::~VolumetricLighting() {
    destroy();
}

bool VolumetricLighting::init(const Config& config) {
    config_ = config;

    // Create volumetric shader
    volumetricShader_ = std::make_shared<Shader>();
    if (!volumetricShader_->compile("volumetric.vert", "volumetric.frag")) {
        std::cerr << "Failed to compile volumetric shaders\n";
        return false;
    }

    // Create blend shader
    blendShader_ = std::make_shared<Shader>();
    if (!blendShader_->compile("fullscreen.vert", "volumetric_blend.frag")) {
        std::cerr << "Failed to compile volumetric blend shader\n";
        return false;
    }

    // Load blue noise texture for dithering
    if (config_.useBlueNoise) {
        loadBlueNoise_();
    }

    // Create framebuffer
    if (!createFramebuffer_(1920, 1080)) {
        std::cerr << "Failed to create volumetric framebuffer\n";
        return false;
    }

    return true;
}

void VolumetricLighting::destroy() {
    if (volumetricFBO_) {
        glDeleteFramebuffers(1, &volumetricFBO_);
        volumetricFBO_ = 0;
    }
    if (volumetricTexture_) {
        glDeleteTextures(1, &volumetricTexture_);
        volumetricTexture_ = 0;
    }
    if (volumetricDepth_) {
        glDeleteRenderbuffers(1, &volumetricDepth_);
        volumetricDepth_ = 0;
    }
}

void VolumetricLighting::setRaySteps(uint32_t steps) {
    config_.raySteps = glm::clamp(steps, 8u, 32u);
}

void VolumetricLighting::setScatteringStrength(float strength) {
    config_.scatteringStrength = glm::clamp(strength, 0.0f, 1.0f);
}

void VolumetricLighting::setStepSize(float size) {
    config_.stepSize = glm::clamp(size, 0.5f, 2.0f);
}

bool VolumetricLighting::createFramebuffer_(uint32_t w, uint32_t h) {
    width_ = w;
    height_ = h;

    glGenFramebuffers(1, &volumetricFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, volumetricFBO_);

    // Create color texture (RGBA16F)
    glGenTextures(1, &volumetricTexture_);
    glBindTexture(GL_TEXTURE_2D, volumetricTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, volumetricTexture_, 0);

    // Create depth renderbuffer
    glGenRenderbuffers(1, &volumetricDepth_);
    glBindRenderbuffer(GL_RENDERBUFFER, volumetricDepth_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, volumetricDepth_);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Volumetric FBO incomplete\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void VolumetricLighting::loadBlueNoise_() {
    // Create simple 1-channel blue noise texture
    // For production, load from file (e.g., 64x64 PNG)
    // For now, use a simple procedural pattern
    blueNoiseTexture_ = std::make_shared<Texture2D>();
    
    uint8_t noiseData[64 * 64];
    for (int i = 0; i < 64 * 64; ++i) {
        noiseData[i] = (uint8_t)((sin(i * 0.1f) + cos(i * 0.05f)) * 127.5f + 127.5f);
    }
    
    glGenTextures(1, &blueNoiseTexture_->textureID);
    glBindTexture(GL_TEXTURE_2D, blueNoiseTexture_->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 64, 64, 0, GL_RED, GL_UNSIGNED_BYTE, noiseData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

GLuint VolumetricLighting::render(GLuint inputDepthTexture, GLuint inputColorTexture,
                                  const glm::mat4& projection, const glm::mat4& invProjection,
                                  const glm::vec3& cameraPos, const glm::vec3& sunDir) {
    glBindFramebuffer(GL_FRAMEBUFFER, volumetricFBO_);
    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    volumetricShader_->use();
    volumetricShader_->setInt("depthTexture", 0);
    volumetricShader_->setInt("colorTexture", 1);
    if (config_.useBlueNoise) {
        volumetricShader_->setInt("blueNoise", 2);
    }
    
    volumetricShader_->setInt("raySteps", (int)config_.raySteps);
    volumetricShader_->setFloat("stepSize", config_.stepSize);
    volumetricShader_->setFloat("scatteringStrength", config_.scatteringStrength);
    volumetricShader_->setFloat("rayStartDistance", config_.rayStartDistance);
    volumetricShader_->setFloat("heightAbsorption", config_.heightAbsorption);
    
    volumetricShader_->setVec3("cameraPos", cameraPos);
    volumetricShader_->setVec3("sunDir", glm::normalize(sunDir));
    volumetricShader_->setVec3("sunColor", config_.sunColor);
    
    volumetricShader_->setMat4("invProjection", invProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputDepthTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, inputColorTexture);
    if (config_.useBlueNoise && blueNoiseTexture_) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, blueNoiseTexture_->textureID);
    }

    // Render fullscreen quad
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);  // Assumes fullscreen quad VAO is bound
    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return volumetricTexture_;
}

void VolumetricLighting::applyToImage(GLuint sceneTexture, GLuint volumetricTexture,
                                     float blendStrength) {
    blendShader_->use();
    blendShader_->setInt("sceneTexture", 0);
    blendShader_->setInt("volumetricTexture", 1);
    blendShader_->setFloat("blendStrength", glm::clamp(blendStrength, 0.0f, 1.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, volumetricTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

}  // namespace RenderLib
