#include "RenderLib/TemporalFilter.hpp"
#include "RenderLib/Shader.hpp"
#include <iostream>

namespace RenderLib {

TemporalFilter::TemporalFilter() {}

TemporalFilter::~TemporalFilter() {
    destroy();
}

bool TemporalFilter::init(uint32_t width, uint32_t height, const Config& config) {
    width_ = width;
    height_ = height;
    config_ = config;

    // Create shader
    temporalFilterShader_ = std::make_shared<Shader>();
    if (!temporalFilterShader_->compile("fullscreen.vert", "temporal_filter.frag")) {
        std::cerr << "Failed to compile temporal filter shader\n";
        return false;
    }

    if (!createFramebuffers_(width, height)) {
        std::cerr << "Failed to create temporal filter framebuffers\n";
        return false;
    }

    return true;
}

void TemporalFilter::destroy() {
    if (temporalFBO_) glDeleteFramebuffers(1, &temporalFBO_);
    if (currentFrameTexture_) glDeleteTextures(1, &currentFrameTexture_);
    if (historyFrameTexture_) glDeleteTextures(1, &historyFrameTexture_);
    if (filteredTexture_) glDeleteTextures(1, &filteredTexture_);
}

bool TemporalFilter::createFramebuffers_(uint32_t w, uint32_t h) {
    // Create textures
    glGenTextures(1, &currentFrameTexture_);
    glBindTexture(GL_TEXTURE_2D, currentFrameTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &historyFrameTexture_);
    glBindTexture(GL_TEXTURE_2D, historyFrameTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &filteredTexture_);
    glBindTexture(GL_TEXTURE_2D, filteredTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // FBO
    glGenFramebuffers(1, &temporalFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, temporalFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, filteredTexture_, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Temporal filter FBO incomplete\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

GLuint TemporalFilter::filter(GLuint currentFrame, GLuint velocityTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, temporalFBO_);
    glViewport(0, 0, width_, height_);

    temporalFilterShader_->use();
    temporalFilterShader_->setInt("currentFrame", 0);
    temporalFilterShader_->setInt("historyFrame", 1);
    if (velocityTexture) {
        temporalFilterShader_->setInt("velocityTexture", 2);
    }

    temporalFilterShader_->setFloat("feedbackMin", config_.feedbackMin);
    temporalFilterShader_->setFloat("feedbackMax", config_.feedbackMax);
    temporalFilterShader_->setFloat("velocityThreshold", config_.velocityThreshold);
    temporalFilterShader_->setBool("useMotionVectors", config_.useMotionVectors && velocityTexture != 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentFrame);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, historyFrameTexture_);

    if (velocityTexture) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, velocityTexture);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return filteredTexture_;
}

void TemporalFilter::swapHistory() {
    // Copy current filtered result to history for next frame
    glCopyImageSubData(
        filteredTexture_, GL_TEXTURE_2D, 0, 0, 0, 0,
        historyFrameTexture_, GL_TEXTURE_2D, 0, 0, 0, 0,
        width_, height_, 1
    );
}

}  // namespace RenderLib
