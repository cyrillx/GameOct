#include "RenderLib/Renderer.hpp"
#include "RenderLib/Shader.hpp"
#include "RenderLib/UBO.hpp"
#include "RenderLib/Mesh.hpp"
#include <iostream>

namespace RenderLib {

ForwardRenderer::ForwardRenderer() {}
ForwardRenderer::~ForwardRenderer() {}

bool ForwardRenderer::init(int width, int height) {
    width_ = width; height_ = height;
    // create HDR framebuffer
    glGenFramebuffers(1, &hdrFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);
    colorBuffer_.createEmpty(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    colorBuffer_.bind(0);
    GLuint rbo; glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "HDR FBO incomplete\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // prepare UBO for lights
    UBO lights;
    lights.create("Lights", 1, 16 * 1024); // 16KB default
    return true;
}

void ForwardRenderer::resize(int w, int h) {
    width_ = w; height_ = h;
    // reallocate textures if needed (omitted)
}

void ForwardRenderer::beginFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);
    glViewport(0,0,width_,height_);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ForwardRenderer::endFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // do postprocessing (HDR/bloom/SSR) - omitted detailed passes here
}

void ForwardRenderer::submitMesh(const Mesh& mesh, const Material& mat, const glm::mat4& model) {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if(currentProgram == 0) return; // no shader bound

    // set model matrix if uniform exists
    GLint locModel = glGetUniformLocation((GLuint)currentProgram, "uModel");
    if(locModel >= 0) {
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    }

    // bind material textures to conventional units
    if(mat.diffuse) {
        mat.diffuse->bind(0);
        GLint loc = glGetUniformLocation((GLuint)currentProgram, "uDiffuse");
        if(loc >= 0) glUniform1i(loc, 0);
    }
    if(mat.specular) {
        mat.specular->bind(1);
        GLint loc = glGetUniformLocation((GLuint)currentProgram, "uSpecular");
        if(loc >= 0) glUniform1i(loc, 1);
    }
    if(mat.normal) {
        mat.normal->bind(2);
        GLint loc = glGetUniformLocation((GLuint)currentProgram, "uNormalMap");
        if(loc >= 0) glUniform1i(loc, 2);
    }

    // set shininess if present
    GLint locSh = glGetUniformLocation((GLuint)currentProgram, "material.shininess");
    if(locSh >= 0) glUniform1f(locSh, mat.shininess);

    // draw mesh (TODO: fix - need to properly pass mesh)
    // mesh.draw();
}

} // namespace RenderLib
