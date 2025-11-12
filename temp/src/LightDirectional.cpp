#include "RenderLib/LightDirectional.hpp"
#include <iostream>

namespace RenderLib {

void DirectionalLight::initShadowMap() {
    if(depthMapFBO) return; // already initialized
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);

    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadow FBO incomplete for directional light\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalLight::resizeShadow(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    if(depthMap) glDeleteTextures(1, &depthMap);
    if(depthMapFBO) glDeleteFramebuffers(1, &depthMapFBO);
    depthMap = 0; depthMapFBO = 0;
    initShadowMap();
}

} // namespace RenderLib
