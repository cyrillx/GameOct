#include "RenderLib/ReflectionProbe.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

namespace RenderLib {

static const glm::vec3 CUBE_TARGETS[6] = {
    glm::vec3(1,0,0), glm::vec3(-1,0,0), glm::vec3(0,1,0),
    glm::vec3(0,-1,0), glm::vec3(0,0,1), glm::vec3(0,0,-1)
};

static const glm::vec3 CUBE_UPS[6] = {
    glm::vec3(0,-1,0), glm::vec3(0,-1,0), glm::vec3(0,0,1),
    glm::vec3(0,0,-1), glm::vec3(0,-1,0), glm::vec3(0,-1,0)
};

ReflectionProbe::ReflectionProbe() = default;

ReflectionProbe::~ReflectionProbe() {
    destroy();
}

bool ReflectionProbe::init(int resolution) {
    resolution_ = resolution;

    glGenTextures(1, &cubemapTex_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex_);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, resolution_, resolution_, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &fbo_);
    return true;
}

void ReflectionProbe::destroy() {
    if (cubemapTex_) glDeleteTextures(1, &cubemapTex_);
    if (fbo_) glDeleteFramebuffers(1, &fbo_);
    cubemapTex_ = 0;
    fbo_ = 0;
}

bool ReflectionProbe::bakeAt(const glm::vec3& position, std::function<void(const glm::mat4&, const glm::mat4&, unsigned)> renderFace) {
    if (!cubemapTex_) return false;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);

    for (unsigned face = 0; face < 6; ++face) {
        glm::mat4 view = glm::lookAt(position, position + CUBE_TARGETS[face], CUBE_UPS[face]);
        // Attach face as color attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubemapTex_, 0);
        glViewport(0, 0, resolution_, resolution_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Let the caller render the scene from this view
        renderFace(view, proj, face);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

} // namespace RenderLib
