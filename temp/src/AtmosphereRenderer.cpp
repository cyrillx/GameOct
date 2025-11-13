#include "RenderLib/AtmosphereRenderer.hpp"
#include "RenderLib/Shader.hpp"

namespace RenderLib {

AtmosphereRenderer::AtmosphereRenderer() = default;

AtmosphereRenderer::~AtmosphereRenderer() {
    destroy();
}

bool AtmosphereRenderer::init(int width, int height) {
    width_ = width;
    height_ = height;

    // Load fog shader
    fogShader_ = std::make_shared<Shader>(
        "shaders/fog.vert",
        "shaders/fog.frag"
    );

    if (!fogShader_) {
        return false;
    }

    createFramebuffers_();
    return true;
}

void AtmosphereRenderer::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    width_ = width;
    height_ = height;
    destroy();
    init(width, height);
}

void AtmosphereRenderer::destroy() {
    if (fogFBO_) glDeleteFramebuffers(1, &fogFBO_);
    if (outputTex_) glDeleteTextures(1, &outputTex_);
}

void AtmosphereRenderer::createFramebuffers_() {
    // Create output texture
    glGenTextures(1, &outputTex_);
    glBindTexture(GL_TEXTURE_2D, outputTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &fogFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, fogFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex_, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AtmosphereRenderer::applyFog(GLuint colorTex, GLuint depthTex, const glm::mat4& view) {
    glBindFramebuffer(GL_FRAMEBUFFER, fogFBO_);
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT);

    fogShader_->use();
    fogShader_->setInt("uColorTex", 0);
    if (config_.useDepthBuffer) {
        fogShader_->setInt("uDepthTex", 1);
    }

    fogShader_->setVec3("uFogColor", config_.fogColor);
    fogShader_->setFloat("uFogDensity", config_.fogDensity);
    fogShader_->setFloat("uFogFalloff", config_.fogFalloff);
    fogShader_->setFloat("uFogNear", config_.fogNear);
    fogShader_->setFloat("uFogFar", config_.fogFar);
    fogShader_->setInt("uFogType", static_cast<int>(config_.fogType));
    
    fogShader_->setBool("uUseHeightFog", config_.useHeightFog);
    fogShader_->setFloat("uHeightFogDensity", config_.heightFogDensity);
    fogShader_->setFloat("uHeightFogFalloff", config_.heightFogFalloff);
    fogShader_->setFloat("uFogBaseHeight", config_.fogBaseHeight);
    
    fogShader_->setFloat("uSkyScattering", config_.skyScattering);
    fogShader_->setVec3("uSunDirection", config_.sunDirection);
    fogShader_->setVec3("uSunColor", config_.sunColor);
    fogShader_->setMat4("uView", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    if (config_.useDepthBuffer) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);
    }

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace RenderLib
