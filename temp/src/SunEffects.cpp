#include "RenderLib/SunEffects.hpp"
#include "RenderLib/Shader.hpp"

namespace RenderLib {

SunEffects::SunEffects() = default;

SunEffects::~SunEffects() {
    destroy();
}

bool SunEffects::init(int width, int height) {
    width_ = width;
    height_ = height;

    // Load shaders
    godRayShader_ = std::make_shared<Shader>(
        "shaders/volumetric.vert",
        "shaders/volumetric.frag"
    );

    lensFlareShader_ = std::make_shared<Shader>(
        "shaders/lens_flare.vert",
        "shaders/lens_flare.frag"
    );

    if (!godRayShader_ || !lensFlareShader_) {
        return false;
    }

    createFramebuffers_();
    return true;
}

void SunEffects::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    width_ = width;
    height_ = height;
    destroy();
    init(width, height);
}

void SunEffects::destroy() {
    if (sunFBO_) glDeleteFramebuffers(1, &sunFBO_);
    if (outputTex_) glDeleteTextures(1, &outputTex_);
    if (godRayTex_) glDeleteTextures(1, &godRayTex_);
    if (lensFlareTex_) glDeleteTextures(1, &lensFlareTex_);
}

void SunEffects::createFramebuffers_() {
    // Output texture
    glGenTextures(1, &outputTex_);
    glBindTexture(GL_TEXTURE_2D, outputTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // God ray texture
    glGenTextures(1, &godRayTex_);
    glBindTexture(GL_TEXTURE_2D, godRayTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_ / 2, height_ / 2, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Lens flare texture
    glGenTextures(1, &lensFlareTex_);
    glBindTexture(GL_TEXTURE_2D, lensFlareTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &sunFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, sunFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex_, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SunEffects::renderGodRays(GLuint colorTex, GLuint depthTex) {
    if (!config_.enableGodRays) return;

    glBindFramebuffer(GL_FRAMEBUFFER, sunFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, godRayTex_, 0);
    glViewport(0, 0, width_ / 2, height_ / 2);
    glClear(GL_COLOR_BUFFER_BIT);

    godRayShader_->use();
    godRayShader_->setInt("uColorTex", 0);
    godRayShader_->setInt("uDepthTex", 1);
    godRayShader_->setVec3("uSunDirection", config_.sunDirection);
    godRayShader_->setVec3("uSunColor", config_.sunColor);
    godRayShader_->setInt("uSamples", config_.godRaySamples);
    godRayShader_->setFloat("uDecay", config_.godRayDecay);
    godRayShader_->setFloat("uExposure", config_.godRayExposure);
    godRayShader_->setFloat("uIntensity", config_.godRayIntensity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glBindVertexArray(0);  // Use screen quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SunEffects::renderLensFlare(const glm::vec2& screenPos) {
    if (!config_.enableLensFlare) return;

    glBindFramebuffer(GL_FRAMEBUFFER, sunFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lensFlareTex_, 0);
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT);

    lensFlareShader_->use();
    lensFlareShader_->setVec2("uSunScreenPos", screenPos);
    lensFlareShader_->setVec3("uSunColor", config_.sunColor);
    lensFlareShader_->setFloat("uIntensity", config_.lensFlareIntensity);

    glBindVertexArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace RenderLib
