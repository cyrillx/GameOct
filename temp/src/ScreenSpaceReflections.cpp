#include "RenderLib/ScreenSpaceReflections.hpp"
#include "RenderLib/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace RenderLib {

ScreenSpaceReflections::ScreenSpaceReflections() = default;

ScreenSpaceReflections::~ScreenSpaceReflections() {
    destroy();
}

bool ScreenSpaceReflections::init(int width, int height) {
    width_ = width;
    height_ = height;

    // Load shaders
    rayMarchShader_ = std::make_shared<Shader>(
        "shaders/ssr.vert",
        "shaders/ssr.frag"
    );
    
    temporalShader_ = std::make_shared<Shader>(
        "shaders/ssr_temporal.vert",
        "shaders/ssr_temporal.frag"
    );

    if (!rayMarchShader_ || !temporalShader_) {
        return false;
    }

    createFramebuffers_();
    return true;
}

void ScreenSpaceReflections::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    width_ = width;
    height_ = height;
    destroy();
    init(width, height);
}

void ScreenSpaceReflections::destroy() {
    if (ssrFBO_) glDeleteFramebuffers(1, &ssrFBO_);
    if (reflectionTex_) glDeleteTextures(1, &reflectionTex_);
    if (reflectionPrevTex_) glDeleteTextures(1, &reflectionPrevTex_);
    if (rayMarchTex_) glDeleteTextures(1, &rayMarchTex_);
    
    ssrFBO_ = 0;
    reflectionTex_ = 0;
    reflectionPrevTex_ = 0;
    rayMarchTex_ = 0;
}

void ScreenSpaceReflections::createFramebuffers_() {
    // Create textures
    glGenTextures(1, &rayMarchTex_);
    glBindTexture(GL_TEXTURE_2D, rayMarchTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &reflectionTex_);
    glBindTexture(GL_TEXTURE_2D, reflectionTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &reflectionPrevTex_);
    glBindTexture(GL_TEXTURE_2D, reflectionPrevTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create FBO
    glGenFramebuffers(1, &ssrFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rayMarchTex_, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScreenSpaceReflections::render(GLuint colorTex, GLuint normalTex, GLuint depthTex,
                                   const glm::mat4& proj, const glm::mat4& view,
                                   const glm::mat4& lastViewProj) {
    // Ray marching pass
    glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO_);
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT);

    rayMarchShader_->use();
    rayMarchShader_->setInt("uColorTex", 0);
    rayMarchShader_->setInt("uNormalTex", 1);
    rayMarchShader_->setInt("uDepthTex", 2);
    rayMarchShader_->setMat4("uProj", proj);
    rayMarchShader_->setMat4("uView", view);
    rayMarchShader_->setInt("uMaxSteps", config_.maxSteps);
    rayMarchShader_->setInt("uMaxBinarySteps", config_.maxBinarySteps);
    rayMarchShader_->setFloat("uRayStep", config_.rayStep);
    rayMarchShader_->setFloat("uMaxDistance", config_.maxDistance);
    rayMarchShader_->setFloat("uThickness", config_.thickness);
    rayMarchShader_->setFloat("uEdgeFade", config_.edgeFade);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Temporal reprojection if enabled
    if (config_.useTemporalReprojection) {
        applyTemporalReprojection_();
    } else {
        std::swap(reflectionTex_, rayMarchTex_);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScreenSpaceReflections::renderRayMarch_() {
    // Implemented in render()
}

void ScreenSpaceReflections::applyTemporalReprojection_() {
    // Blend with previous frame for temporal coherence
    glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex_, 0);

    temporalShader_->use();
    temporalShader_->setInt("uCurrent", 0);
    temporalShader_->setInt("uPrevious", 1);
    temporalShader_->setFloat("uBlendFactor", 0.1f);  // 90% previous, 10% current

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rayMarchTex_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, reflectionPrevTex_);

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Swap for next frame
    std::swap(reflectionTex_, reflectionPrevTex_);
}

} // namespace RenderLib
