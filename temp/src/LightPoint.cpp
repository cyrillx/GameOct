#include "RenderLib/LightPoint.hpp"
#include <iostream>

namespace RenderLib {

void PointLight::initShadowCube() {
    if(depthCubeFBO) return;
    glGenFramebuffers(1, &depthCubeFBO);
    glGenTextures(1, &depthCubeMap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    for(int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F,
                     shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, depthCubeFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadow cubeFBO incomplete for point light\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLight::resizeShadow(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    if(depthCubeMap) glDeleteTextures(1, &depthCubeMap);
    if(depthCubeFBO) glDeleteFramebuffers(1, &depthCubeFBO);
    depthCubeMap = 0; depthCubeFBO = 0;
    initShadowCube();
}

} // namespace RenderLib
