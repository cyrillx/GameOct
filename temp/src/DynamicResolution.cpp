#include "RenderLib/DynamicResolution.hpp"
#include "RenderLib/Shader.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>

namespace RenderLib {

DynamicResolution::DynamicResolution() {}

DynamicResolution::~DynamicResolution() {
    destroy();
}

bool DynamicResolution::init(uint32_t baseWidth, uint32_t baseHeight, const Config& config) {
    baseWidth_ = baseWidth;
    baseHeight_ = baseHeight;
    config_ = config;
    
    targetFrameTime_ = 1.0f / config_.targetFPS;
    currentScale_ = config_.maxResolutionScale;

    // Create upscale shader
    upscaleShader_ = std::make_shared<Shader>();
    if (!upscaleShader_->compile("fullscreen.vert", "upscale.frag")) {
        std::cerr << "Failed to compile upscale shader\n";
        return false;
    }

    if (!createUpscaleResources_()) {
        std::cerr << "Failed to create upscale resources\n";
        return false;
    }

    return true;
}

void DynamicResolution::destroy() {
    if (upscaleFBO_) {
        glDeleteFramebuffers(1, &upscaleFBO_);
        upscaleFBO_ = 0;
    }
}

bool DynamicResolution::createUpscaleResources_() {
    glGenFramebuffers(1, &upscaleFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, upscaleFBO_);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

glm::vec2 DynamicResolution::getRenderDimensions() const {
    return glm::vec2(
        baseWidth_ * currentScale_,
        baseHeight_ * currentScale_
    );
}

glm::vec2 DynamicResolution::getDisplayDimensions() const {
    return glm::vec2(baseWidth_, baseHeight_);
}

void DynamicResolution::update(float gpuTime, float cpuTime) {
    // Running average
    avgGPUTime_ = avgGPUTime_ * 0.8f + gpuTime * 0.2f;
    avgCPUTime_ = avgCPUTime_ * 0.8f + cpuTime * 0.2f;

    float totalTime = avgGPUTime_ + avgCPUTime_;

    stabilizeCounter_++;

    // Don't adjust every frame - wait for stability
    if (stabilizeCounter_ < config_.stabilizeFrames) {
        stats_.avgGPUTime = avgGPUTime_;
        stats_.avgCPUTime = avgCPUTime_;
        stats_.currentScale = currentScale_;
        return;
    }

    stabilizeCounter_ = 0;

    // Adjust resolution based on frame time
    if (totalTime > targetFrameTime_ * 1.1f) {
        // Too slow - decrease resolution
        currentScale_ = std::max(config_.minResolutionScale,
                               currentScale_ - config_.scaleStep);
        stats_.scaleAdjustments++;
    } else if (totalTime < targetFrameTime_ * 0.9f) {
        // Too fast - increase resolution
        currentScale_ = std::min(config_.maxResolutionScale,
                               currentScale_ + config_.scaleStep);
        stats_.scaleAdjustments++;
    }

    stats_.avgGPUTime = avgGPUTime_;
    stats_.avgCPUTime = avgCPUTime_;
    stats_.currentScale = currentScale_;
}

void DynamicResolution::upscaleToDisplay(GLuint lowResTexture, GLuint displayFramebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);

    upscaleShader_->use();
    upscaleShader_->setInt("inputTexture", 0);
    upscaleShader_->setVec2("inputSize", getRenderDimensions());
    upscaleShader_->setVec2("outputSize", getDisplayDimensions());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lowResTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

}  // namespace RenderLib
