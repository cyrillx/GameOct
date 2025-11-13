#include "RenderLib/Water.hpp"

namespace RenderLib {

Water::Water() = default;
Water::~Water() = default;

bool Water::init(const Config& config) {
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

    return true;
}

void Water::updateWaves(float deltaTime, float speed) {
    normalOffset_ += glm::vec2(speed * deltaTime, speed * deltaTime * 0.7f);

    // Wrap offset to [0, 1]
    if (normalOffset_.x > 1.0f) normalOffset_.x -= 1.0f;
    if (normalOffset_.y > 1.0f) normalOffset_.y -= 1.0f;
}

void Water::render(const glm::mat4& projection, const glm::mat4& view) {
    if (!mesh_) return;

    // TODO: Bind shader with water-specific uniforms
    // - Projection, view matrices
    // - Normal textures (2 layers with offset)
    // - Reflection texture
    // - Depth texture
    // - Fresnel, depth scale, colors

    mesh_->draw();
}

} // namespace RenderLib
