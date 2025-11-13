#include "RenderLib/TemporalSSAO.hpp"
#include "RenderLib/Shader.hpp"
#include <iostream>

namespace RenderLib {

TemporalSSAO::TemporalSSAO() {}

TemporalSSAO::~TemporalSSAO() {
    destroy();
}

bool TemporalSSAO::init(uint32_t width, uint32_t height, const Config& config) {
    width_ = width;
    height_ = height;
    config_ = config;

    // Create shaders
    ssaoShader_ = std::make_shared<Shader>();
    if (!ssaoShader_->compile("ssao.vert", "ssao_temporal.frag")) {
        std::cerr << "Failed to compile temporal SSAO shader\n";
        return false;
    }

    blurShader_ = std::make_shared<Shader>();
    if (!blurShader_->compile("fullscreen.vert", "blur.frag")) {
        std::cerr << "Failed to compile blur shader\n";
        return false;
    }

    temporalShader_ = std::make_shared<Shader>();
    if (!temporalShader_->compile("fullscreen.vert", "temporal_blend.frag")) {
        std::cerr << "Failed to compile temporal blend shader\n";
        return false;
    }

    if (!createFramebuffers_(width, height)) {
        std::cerr << "Failed to create SSAO framebuffers\n";
        return false;
    }

    return true;
}

void TemporalSSAO::destroy() {
    if (ssaoFBO_) glDeleteFramebuffers(1, &ssaoFBO_);
    if (blurFBO_) glDeleteFramebuffers(1, &blurFBO_);
    if (ssaoTexture_) glDeleteTextures(1, &ssaoTexture_);
    if (ssaoHistoryTexture_) glDeleteTextures(1, &ssaoHistoryTexture_);
    if (blurredSSAO_) glDeleteTextures(1, &blurredSSAO_);
    if (temporalSSAO_) glDeleteTextures(1, &temporalSSAO_);
}

bool TemporalSSAO::createFramebuffers_(uint32_t w, uint32_t h) {
    // SSAO render target
    glGenFramebuffers(1, &ssaoFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO_);

    glGenTextures(1, &ssaoTexture_);
    glBindTexture(GL_TEXTURE_2D, ssaoTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture_, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "SSAO FBO incomplete\n";
        return false;
    }

    // History texture (for temporal filtering)
    glGenTextures(1, &ssaoHistoryTexture_);
    glBindTexture(GL_TEXTURE_2D, ssaoHistoryTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Blur FBO
    glGenFramebuffers(1, &blurFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_);

    glGenTextures(1, &blurredSSAO_);
    glBindTexture(GL_TEXTURE_2D, blurredSSAO_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurredSSAO_, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Blur FBO incomplete\n";
        return false;
    }

    // Temporal blend FBO
    glGenTextures(1, &temporalSSAO_);
    glBindTexture(GL_TEXTURE_2D, temporalSSAO_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

GLuint TemporalSSAO::render(GLuint depthTexture, const glm::mat4& projection,
                            const glm::mat4& invProjection) {
    // Step 1: Render SSAO
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO_);
    glViewport(0, 0, width_, height_);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ssaoShader_->use();
    ssaoShader_->setInt("depthTexture", 0);
    ssaoShader_->setInt("sampleCount", (int)config_.sampleCount);
    ssaoShader_->setFloat("radius", config_.radius);
    ssaoShader_->setFloat("bias", config_.bias);
    ssaoShader_->setMat4("projection", projection);
    ssaoShader_->setMat4("invProjection", invProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Step 2: Blur
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO_);
    blurShader_->use();
    blurShader_->setInt("inputTexture", 0);
    blurShader_->setFloat("blurRadius", config_.blurRadius);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoTexture_);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Step 3: Temporal blend
    // (In render loop, blend blurredSSAO_ with ssaoHistoryTexture_)

    postProcess();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return temporalSSAO_;
}

void TemporalSSAO::postProcess() {
    // Blend with history
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Render to screen for now
    
    temporalShader_->use();
    temporalShader_->setInt("currentSSAO", 0);
    temporalShader_->setInt("historySSAO", 1);
    temporalShader_->setFloat("temporalWeight", config_.temporalWeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blurredSSAO_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssaoHistoryTexture_);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    swapHistoryTexture_();
}

void TemporalSSAO::swapHistoryTexture_() {
    // Copy current to history for next frame
    glCopyImageSubData(
        blurredSSAO_, GL_TEXTURE_2D, 0, 0, 0, 0,
        ssaoHistoryTexture_, GL_TEXTURE_2D, 0, 0, 0, 0,
        width_, height_, 1
    );
}

void TemporalSSAO::setBlurStrength(float strength) {
    config_.blurRadius = glm::clamp(strength, 2.0f, 8.0f);
}

void TemporalSSAO::setTemporalWeight(float weight) {
    config_.temporalWeight = glm::clamp(weight, 0.0f, 1.0f);
}

}  // namespace RenderLib
