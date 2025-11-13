#include "RenderLib/SSIL.hpp"
#include "RenderLib/Shader.hpp"
#include <iostream>

namespace RenderLib {

SSIL::SSIL() = default;

SSIL::~SSIL() {
    destroy();
}

bool SSIL::init(int width, int height) {
    width_ = width;
    height_ = height;

    ssilShader_ = std::make_shared<Shader>("shaders/ssil.vert", "shaders/ssil.frag");
    if (!ssilShader_) {
        std::cerr << "Failed to load SSIL shaders\n";
        return false;
    }

    // Create target texture
    glGenTextures(1, &indirectTex_);
    glBindTexture(GL_TEXTURE_2D, indirectTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &ssilFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, ssilFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, indirectTex_, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "SSIL FBO incomplete\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void SSIL::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    destroy();
    init(width, height);
}

void SSIL::destroy() {
    if (indirectTex_) glDeleteTextures(1, &indirectTex_);
    if (ssilFBO_) glDeleteFramebuffers(1, &ssilFBO_);
    indirectTex_ = 0;
    ssilFBO_ = 0;
}

void SSIL::render(GLuint colorTex, GLuint normalTex, GLuint depthTex, GLuint positionTex,
                  const glm::mat4& proj, const glm::mat4& view) {
    if (!ssilShader_) return;

    glBindFramebuffer(GL_FRAMEBUFFER, ssilFBO_);
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT);

    ssilShader_->use();
    ssilShader_->setInt("uColorTex", 0);
    ssilShader_->setInt("uNormalTex", 1);
    ssilShader_->setInt("uDepthTex", 2);
    ssilShader_->setInt("uPositionTex", 3);

    ssilShader_->setInt("uSampleCount", config_.sampleCount);
    ssilShader_->setFloat("uSampleRadius", config_.sampleRadius);
    ssilShader_->setFloat("uFalloff", config_.falloff);
    ssilShader_->setBool("uUseBentNormals", config_.useBentNormals);

    ssilShader_->setMat4("uProj", proj);
    ssilShader_->setMat4("uView", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, positionTex);

    // Draw full-screen quad (assumes VAO 0 quad is set up elsewhere)
    glBindVertexArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace RenderLib
