#include "RenderLib/BloomPostProcess.hpp"
#include "RenderLib/Shader.hpp"

namespace RenderLib {

BloomPostProcess::BloomPostProcess() = default;

BloomPostProcess::~BloomPostProcess() {
    destroy();
}

bool BloomPostProcess::init(int width, int height) {
    width_ = width;
    height_ = height;

    // Load shaders
    extractShader_ = std::make_shared<Shader>(
        "shaders/extract_bright.vert",
        "shaders/extract_bright.frag"
    );
    
    blurShader_ = std::make_shared<Shader>(
        "shaders/blur.vert",
        "shaders/blur.frag"
    );

    if (!extractShader_ || !blurShader_) {
        return false;
    }

    createFramebuffers_();
    return true;
}

void BloomPostProcess::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    width_ = width;
    height_ = height;
    destroy();
    init(width, height);
}

void BloomPostProcess::destroy() {
    if (brightFBO_) glDeleteFramebuffers(1, &brightFBO_);
    if (bloomFBO_) glDeleteFramebuffers(1, &bloomFBO_);
    if (brightTex_) glDeleteTextures(1, &brightTex_);
    if (bloomTex_) glDeleteTextures(1, &bloomTex_);
    if (blurTex_[0]) glDeleteTextures(1, &blurTex_[0]);
    if (blurTex_[1]) glDeleteTextures(1, &blurTex_[1]);
}

void BloomPostProcess::createFramebuffers_() {
    // Bright extraction texture (half resolution)
    glGenTextures(1, &brightTex_);
    glBindTexture(GL_TEXTURE_2D, brightTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_ / 2, height_ / 2, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &brightFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, brightFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brightTex_, 0);

    // Bloom result texture
    glGenTextures(1, &bloomTex_);
    glBindTexture(GL_TEXTURE_2D, bloomTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Blur textures (ping-pong)
    for (int i = 0; i < 2; ++i) {
        glGenTextures(1, &blurTex_[i]);
        glBindTexture(GL_TEXTURE_2D, blurTex_[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_ / 2, height_ / 2, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glGenFramebuffers(1, &bloomFBO_);
}

void BloomPostProcess::render(GLuint colorTex) {
    // Step 1: Extract bright pixels
    extractBright_();

    // Step 2: Blur in hierarchy (upsample)
    blurHierarchy_();
}

void BloomPostProcess::extractBright_() {
    glBindFramebuffer(GL_FRAMEBUFFER, brightFBO_);
    glViewport(0, 0, width_ / 2, height_ / 2);
    glClear(GL_COLOR_BUFFER_BIT);

    extractShader_->use();
    extractShader_->setInt("uColorTex", 0);
    extractShader_->setFloat("uThreshold", config_.threshold);
    extractShader_->setFloat("uMaxBrightness", config_.maxBrightness);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bloomTex_);  // Input

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void BloomPostProcess::blurHierarchy_() {
    // Multi-pass Gaussian blur with progressive upsampling
    GLuint current = brightTex_;

    for (int iter = 0; iter < config_.iterations; ++iter) {
        // Horizontal blur
        blur_(current, blurTex_[0], true);
        // Vertical blur
        blur_(blurTex_[0], blurTex_[1], false);
        current = blurTex_[1];
    }

    // Copy final result to bloom texture
    // (Usually done via render-to-texture or copy)
}

void BloomPostProcess::blur_(GLuint input, GLuint output, bool horizontal) {
    glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);
    glViewport(0, 0, width_ / 2, height_ / 2);

    blurShader_->use();
    blurShader_->setInt("uTexture", 0);
    blurShader_->setInt("uHorizontal", horizontal ? 1 : 0);
    blurShader_->setFloat("uImageWidth", width_ / 2.0f);
    blurShader_->setFloat("uImageHeight", height_ / 2.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, input);

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace RenderLib
