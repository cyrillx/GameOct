#include "RenderLib/MirrorWater.hpp"
#include "RenderLib/Renderer.hpp"
#include <glad/glad.h>

namespace RenderLib {

MirrorWater::MirrorWater() = default;
MirrorWater::~MirrorWater() {
    if (reflectionFBO_) {
        glDeleteFramebuffers(1, &reflectionFBO_);
    }
}

bool MirrorWater::init(const Config& config) {
    config_ = config;
    normalOffset_ = glm::vec2(0.0f);

    // Create quad mesh for water plane
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;

    float hw = config.width / 2.0f;
    float hh = config.height / 2.0f;

    vertices.push_back({glm::vec3(-hw, 0, -hh), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(hw, 0, -hh), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(hw, 0, hh), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(1, 0, 0)});
    vertices.push_back({glm::vec3(-hw, 0, hh), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0)});

    indices = {0, 2, 1, 0, 3, 2};

    mesh_ = std::make_unique<Mesh>();
    if (!mesh_->create(vertices, indices)) {
        return false;
    }

    // Create reflection framebuffer and texture
    reflectionTex_ = std::make_unique<Texture2D>();
    reflectionTex_->createEmpty(config.reflectionTexSize, config.reflectionTexSize,
                               GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

    glGenFramebuffers(1, &reflectionFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO_);

    GLuint depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                         config.reflectionTexSize, config.reflectionTexSize);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                          reflectionTex_->id(), 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

glm::mat4 MirrorWater::getMirrorMatrix_(float waterHeight) const {
    // Mirror matrix: reflect across water plane (y = waterHeight)
    glm::mat4 mirror(1.0f);
    mirror[1][1] = -1.0f;  // Flip Y
    mirror[1][3] = 2.0f * waterHeight;  // Offset

    return mirror;
}

void MirrorWater::updateWaves(float deltaTime, float speed) {
    normalOffset_ += glm::vec2(speed * deltaTime, speed * deltaTime * 0.7f);

    if (normalOffset_.x > 1.0f) normalOffset_.x -= 1.0f;
    if (normalOffset_.y > 1.0f) normalOffset_.y -= 1.0f;
}

void MirrorWater::renderReflection(ForwardRenderer* renderer,
                                   const glm::mat4& projection,
                                   const glm::mat4& view) {
    if (!renderer || !reflectionFBO_) return;

    // Create mirrored view matrix
    glm::mat4 mirrorView = getMirrorMatrix_(config_.position.y) * view;

    // Bind reflection framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO_);
    glViewport(0, 0, config_.reflectionTexSize, config_.reflectionTexSize);

    glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render scene from mirror perspective (inverted view)
    // TODO: Call renderer to render all geometry with mirrored view
    // renderer->renderWithView(projection, mirrorView);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MirrorWater::render(const glm::mat4& projection, const glm::mat4& view) {
    if (!mesh_) return;

    // TODO: Bind shader with water-specific uniforms
    // - Projection, view matrices
    // - Normal texture with offset
    // - Reflection texture (from renderReflection)
    // - Fresnel, depth scale, colors

    mesh_->draw();
}

} // namespace RenderLib
