#include "RenderLib/EdgeDetection.hpp"
#include "RenderLib/Shader.hpp"

namespace RenderLib {

EdgeDetection::EdgeDetection() = default;

EdgeDetection::~EdgeDetection() {
    destroy();
}

bool EdgeDetection::init(int width, int height) {
    width_ = width;
    height_ = height;

    // Load edge detection shader
    edgeShader_ = std::make_shared<Shader>(
        "shaders/edge_detect.vert",
        "shaders/edge_detect.frag"
    );

    // Load composite shader
    compositeShader_ = std::make_shared<Shader>(
        "shaders/edge_composite.vert",
        "shaders/edge_composite.frag"
    );

    if (!edgeShader_ || !compositeShader_) {
        return false;
    }

    createFramebuffers_();
    return true;
}

void EdgeDetection::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    width_ = width;
    height_ = height;
    destroy();
    init(width, height);
}

void EdgeDetection::destroy() {
    if (edgeFBO_) glDeleteFramebuffers(1, &edgeFBO_);
    if (compositeFBO_) glDeleteFramebuffers(1, &compositeFBO_);
    if (edgeTex_) glDeleteTextures(1, &edgeTex_);
    if (compositeTex_) glDeleteTextures(1, &compositeTex_);
}

void EdgeDetection::createFramebuffers_() {
    // Edge detection texture
    glGenTextures(1, &edgeTex_);
    glBindTexture(GL_TEXTURE_2D, edgeTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &edgeFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, edgeFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edgeTex_, 0);

    // Composite texture
    glGenTextures(1, &compositeTex_);
    glBindTexture(GL_TEXTURE_2D, compositeTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &compositeFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, compositeTex_, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EdgeDetection::detectEdges(GLuint depthTex, GLuint normalTex) {
    glBindFramebuffer(GL_FRAMEBUFFER, edgeFBO_);
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT);

    edgeShader_->use();
    edgeShader_->setInt("uDepthTex", 0);
    if (normalTex != 0) {
        edgeShader_->setInt("uNormalTex", 1);
    }
    edgeShader_->setInt("uMethod", static_cast<int>(config_.method));
    edgeShader_->setFloat("uEdgeThreshold", config_.edgeThreshold);
    edgeShader_->setFloat("uEdgeStrength", config_.edgeStrength);
    edgeShader_->setVec3("uEdgeColor", config_.edgeColor);
    edgeShader_->setBool("uUseNormalEdges", config_.useNormalEdges);
    edgeShader_->setBool("uUseDepthEdges", config_.useDepthEdges);
    edgeShader_->setFloat("uDepthSensitivity", config_.depthSensitivity);
    edgeShader_->setFloat("uNormalSensitivity", config_.normalSensitivity);
    edgeShader_->setBool("uInvertEdges", config_.invertEdges);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    if (normalTex != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTex);
    }

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void EdgeDetection::compositeWithColor(GLuint colorTex, float blendAmount) {
    glBindFramebuffer(GL_FRAMEBUFFER, compositeFBO_);
    glViewport(0, 0, width_, height_);
    glClear(GL_COLOR_BUFFER_BIT);

    compositeShader_->use();
    compositeShader_->setInt("uColorTex", 0);
    compositeShader_->setInt("uEdgeTex", 1);
    compositeShader_->setFloat("uBlendAmount", blendAmount);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, edgeTex_);

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace RenderLib
